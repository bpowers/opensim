//===--- IOxml.cpp - Base class for interacting with models ----------===//
//
// Copyright 2008 Bobby Powers
//
// This file is part of OpenSim.
// 
// OpenSim is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// OpenSim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
//
//===---------------------------------------------------------------------===//
//
// This class represents models at a high level, suitible for inclusion
// in projects as part of a library.
// TODO: implement features for dynamically changing models.
//
//===---------------------------------------------------------------------===//

// libxml parsing
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "string.h"

#include "model-ioxml.h"

#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define MODEL_IOXML_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_IOXML, ModelIOxmlPrivate))

static gpointer model_ioxml_parent_class = NULL;
static void model_ioxml_init(ModelIOxml *self);
static void model_ioxml_class_init(ModelIOxmlClass *kclass);
static void model_ioxml_dispose(GObject *gobject);
static void model_ioxml_finalize(GObject *gobject);
static int parse_input(xmlDocPtr doc, xmlNodePtr mod);

/* for object properties */
enum
{
  PROP_0,
  PROP_MODEL_NAME,
  PROP_FILE_NAME
};


struct _ModelIOxmlPrivate
{
  gchar    *model_name;
  gchar    *file_name;
  
  gboolean  valid;
};



static gchar *
trim(gchar *str)
{
  if (str == NULL) return NULL;
  if (!g_utf8_validate(str, -1, NULL))
  {
    g_fprintf(stderr, "Error: a string to trim wasn't valid UTF-8.\n");
    return NULL;
  }

  glong length = g_utf8_strlen(str, -1);
  glong clength = strlen(str);
  
  //g_fprintf(stdout, "**len: %d**\n", length);
  
  int start = 0;
  gchar *startp = NULL;
  int end = 0;
  
  gchar *tr;
  for (tr = str; *tr != '\0'; tr = g_utf8_next_char(tr))
  {
    if (*tr != ' ' && *tr != '\t' && *tr != '\n' && *tr != '\r')
      break;
      
    start++;
  }
  
  startp = tr;
  
  // now point to last charactor in the string
  // FIXME: this assumes the last character is not unicode.
  tr = str + length - sizeof(gchar);
  for (end = length; end != 0; --end)
  {
    if (*tr != ' ' && *tr != '\t' && *tr != '\n' && *tr != '\r')
      break;
    
    tr = g_utf8_prev_char(tr);
  }
  
  //g_fprintf(stdout, " *sta: %d*\n", start);
  //g_fprintf(stdout, " *end: %d*\n", length-1-end);
  //g_fprintf(stderr, " *len: %d ('%c' '%c')*\n", tr-startp+sizeof(gchar), *startp, *tr);
  
  // end - start + 1 for the current char
  gsize new_size = tr - startp + sizeof(gchar);
  // +1 is for null termination
  gchar *ret_val = (gchar *)g_malloc(new_size+1);
  strncpy(ret_val, startp, new_size);
  
  ret_val[new_size] = '\0';
  
  g_free(str);
  
  return ret_val;
}



GType 
model_ioxml_get_type()
{
  static GType g_define_type_id = 0; 
  if (G_UNLIKELY(g_define_type_id == 0)) 
  { 
    static const GTypeInfo g_define_type_info = { 
      sizeof (ModelIOxmlClass), 
      (GBaseInitFunc) NULL, 
      (GBaseFinalizeFunc) NULL, 
      (GClassInitFunc) model_ioxml_class_init, 
      (GClassFinalizeFunc) NULL, 
      NULL,   // class_data 
      sizeof (ModelIOxml), 
      0,      // n_preallocs 
      (GInstanceInitFunc) model_ioxml_init, 
    }; 
    g_define_type_id = g_type_register_static(G_TYPE_OBJECT, 
                                              "ModelIOxmlType", 
                                              &g_define_type_info, 
                                              (GTypeFlags) 0); 
  } 
  return g_define_type_id; 

}



void
model_ioxml_set_property(GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  ModelIOxml *self = MODEL_IOXML(object);

  switch (property_id)
  {
  case PROP_MODEL_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->model_name);
    self->priv->model_name = g_value_dup_string(value);
    break;
  case PROP_FILE_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->file_name);
    self->priv->file_name = g_value_dup_string(value);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



void
model_ioxml_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  ModelIOxml *self = MODEL_IOXML(object);

  switch (property_id)
  {
  case PROP_MODEL_NAME:
    g_value_set_string(value, self->priv->model_name);
    break;
  case PROP_FILE_NAME:
    g_value_set_string(value, self->priv->file_name);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
model_ioxml_class_init(ModelIOxmlClass *kclass)
{
  model_ioxml_parent_class = g_type_class_peek_parent(kclass);

  g_type_class_add_private(kclass, sizeof (ModelIOxmlPrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(kclass);
  GParamSpec *model_param_spec;
  
  gobject_class->set_property = model_ioxml_set_property;
  gobject_class->get_property = model_ioxml_get_property;
  gobject_class->dispose      = model_ioxml_dispose;
  gobject_class->finalize     = model_ioxml_finalize;

  model_param_spec = g_param_spec_string("model_name",
                                         "model name",
                                         "Set model's name",
                                         "unnamed model" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_MODEL_NAME,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("file_name",
                                         "full path to file",
                                         "Where the model is saved to",
                                         "" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_FILE_NAME,
                                  model_param_spec);

}



static void
model_ioxml_init(ModelIOxml *self)
{
  self->priv = MODEL_IOXML_GET_PRIVATE(self);
  
  self->priv->valid = TRUE;
}



static void
model_ioxml_dispose(GObject *gobject)
{
  ModelIOxml *self = MODEL_IOXML(gobject);

  /* 
   * In dispose, you are supposed to free all typesecifier before 'IOVenText'
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */

  /* dispose might be called multiple times, so we must guard against
   * calling g_object_unref() on an invalid GObject.
   */
  //if (self->priv->an_object)
  //{
  //  g_object_unref (self->priv->an_object);
  //
  //  self->priv->an_object = NULL;
  //}

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_ioxml_parent_class)->dispose(gobject);
}



static void
model_ioxml_finalize(GObject *gobject)
{
  ModelIOxml *self = MODEL_IOXML(gobject);

  // free g_values and such.
  g_free(self->priv->file_name);
  g_free(self->priv->model_name);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_ioxml_parent_class)->finalize(gobject);
}



int 
model_ioxml_load(ModelIOxml *ioxml, gchar *model_path)
{
  g_object_set(G_OBJECT(ioxml), "file_name", model_path, NULL);
  
  ioxml->priv->valid = FALSE;
  // management of our input
  xmlDocPtr  doc = NULL;
  xmlNodePtr cur = NULL;
  xmlNodePtr sub = NULL;
  xmlNodePtr mod = NULL;
  xmlChar *txt;
    
  doc = xmlParseFile(model_path);
  // then check to see if its a valid xml document
  if (!doc)
  {
    fprintf(stderr, "Error: Document not parsed successfully.\n");
    xmlFreeDoc(doc);
    return;
  }
    
    
  cur = xmlDocGetRootElement(doc);
  // now we get the root element
  if (!cur)
  {
    fprintf(stderr, "Error: Document has no root element.\n");
    xmlFreeDoc(doc);
    return;
  }
    
    
  // and make sure the root element is an opensim tag.  basically,
  // now that we know its XML we want to make sure its OUR xml
  if (xmlStrcmp(cur->name, (const xmlChar *)"opensim"))
  {
    fprintf(stderr, "Error: Document of the wrong type, root node != opensim\n");
    xmlFreeDoc(doc);
    return;
  }
    
    
  // this isn't too important yet, but since its not hard right now
  // lets build in a check for the version of the savefile we're using.
  // hopefully this futureproofs us a little, when I realize that we're 
  // doing things in a bass ackwards way.
  txt = xmlGetProp(cur, (const xmlChar *)"markup");
  if (!xmlStrEqual(txt, (const xmlChar *)"1.0"))
  {
    fprintf(stderr, "Error: Markup must be version 1.0\n");
    xmlFree(txt);
    xmlFreeDoc(doc);
    return;
  }
  else 
  {
    //fprintf(stderr, "Using openSim markup v%s\n", txt);
    xmlFree(txt);
  }


  // now we'll get the (first) model in the file
  for (sub = cur->children; sub != NULL; sub = sub->next)
  {
    if (xmlStrEqual(sub->name, (const xmlChar *)"model"))
    {
      mod = sub;
      break;
    }
  }
  
    
  // and make sure that we've got a pointer to it, and not just 
  // the end of the file.
  if (mod == NULL)
  {
    fprintf(stderr, "Error: No 'model' node.\n");
    xmlFreeDoc(doc);
    return;
  }
  
  gboolean haveModelName = FALSE;
    
  for (cur = mod->children; cur != NULL; cur = cur->next)
  {
    if (xmlStrEqual(cur->name, (const xmlChar *)"name"))
    {
      if (haveModelName)
      {
        fprintf(stderr, "Error: A model can only have one name.\n");
        return;
      }
      else 
        haveModelName = TRUE;
    
      txt = xmlNodeListGetString(doc, cur->children, 0);
      if (txt)
      {
        g_object_set(G_OBJECT(ioxml), "model_name", txt, NULL);
      }
      xmlFree( txt );
    }
    
    if (xmlStrEqual(cur->name, (const xmlChar *)"var"))
    {
      //Variable *ourVar = NULL;
      gchar *var_name;
      gchar *equation;
          
      for (sub = cur->children; sub != NULL; sub = sub->next)
      {
        if (xmlStrEqual(sub->name, (const xmlChar *)"name"))
        {
          txt = xmlNodeListGetString(doc, sub->children, 0);
          var_name = g_strdup(txt);
          var_name = trim(var_name);
          xmlFree( txt );

          continue;
        }
        
        if (xmlStrEqual(sub->name, (const xmlChar *)"equation"))
        {
          txt = xmlNodeListGetString(doc, sub->children, 0);
          equation = g_strdup(txt);
          equation = trim(equation);
          xmlFree( txt );
          
          continue;
        }
      }
      
      //ourVar = new Variable(var_name, equation);
        
      if (var_name != NULL)
      {
        g_fprintf(stderr, "  var '%s'\n    '%s'\n", var_name, equation);
        g_free(var_name);
        g_free(equation);
        //vars[var_name] = ourVar;
      }
      else
      {
        fprintf(stdout, "Error: problem parsing variable.\n");
      }
    }
  }

  // close the file
  xmlFreeDoc(doc);
  
  // *** right now we're assumming that just by having a 
  // validly parsed file, we have a valid equation... *** //
  ioxml->priv->valid = TRUE;
}

