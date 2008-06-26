//===--- IOxml.cpp - Reads models from XML files -------------------------===//
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
// This reads in models from XML save files.
// TODO: save to xml
//
//===---------------------------------------------------------------------===//

#include "IOxml.h"

#include "../Variable.h"

#include <cstdio>

using std::string;
using std::vector;
using std::map;
    

static std::string 
trim(std::string &s, const std::string &drop = " \r\n\t")
{
  std::string r=s.erase(s.find_last_not_of(drop)+1);
  return r.erase(0,r.find_first_not_of(drop));
}



OpenSim::IOxml::IOxml(std::string filePath)
{
  valid = false;
  // management of our input
  xmlDocPtr  doc = NULL;
  xmlNodePtr cur = NULL;
  xmlNodePtr sub = NULL;
  xmlNodePtr mod = NULL;
  xmlChar *txt;
    
  doc = xmlParseFile(filePath.c_str());
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
    
  ParseInput(doc, mod);

  // close the file
  xmlFreeDoc(doc);
  
  // *** right now we're assumming that just by having a 
  // validly parsed file, we have a valid equation... *** //
  valid = true;
}



OpenSim::IOxml::IOxml(std::string filePath, char read_write, bool partial, 
                      std::map<std::string, OpenSim::Variable *> vars, 
                      std::string model_name)
{
  FILE * save_file = fopen(filePath.c_str(), "w");

  if (save_file != NULL)
  {
    fprintf(stdout, "**saving\n");
    fprintf(save_file, "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
\n\
<opensim markup=\"1.0\">\n\
<model>\n\
  <name>%s</name>\n\n", model_name.c_str());

    // loop through vars here.

    fprintf(save_file, "\n</model>\n");
    if (!partial) fprintf(save_file, "\n</opensim>\n");

    fclose(save_file);
  }
  fprintf(stdout, "**\n\nAWESOME THIS IS WHERE TEXT SAVING WILL GO\n\n**\n");
}


OpenSim::IOxml::~IOxml() 
{
}



void 
OpenSim::IOxml::ParseInput(xmlDocPtr doc, xmlNodePtr mod)
{
  bool haveModelName = false;
  xmlChar *txt;
  xmlNodePtr cur, sub;
    
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
        haveModelName = true;
    
      txt = xmlNodeListGetString(doc, cur->children, 0);
      name = (char *)txt;
      xmlFree( txt );
    }
    
    if (xmlStrEqual(cur->name, (const xmlChar *)"var"))
    {
      Variable *ourVar = NULL;
      string varName;
      string equation;
          
      for (sub = cur->children; sub != NULL; sub = sub->next)
      {
        if (xmlStrEqual(sub->name, (const xmlChar *)"name"))
        {
          txt = xmlNodeListGetString(doc, sub->children, 0);
          varName = (char *)txt;
          varName = trim(varName);
          xmlFree( txt );

          continue;
        }
        
        if (xmlStrEqual(sub->name, (const xmlChar *)"equation"))
        {
          txt = xmlNodeListGetString(doc, sub->children, 0);
          equation = (char *)txt;
          equation = trim(equation);
          xmlFree( txt );
          
          continue;
        }
      }
      
      ourVar = new Variable(varName, equation);
        
      if (ourVar && varName != "")
      {
        vars[varName] = ourVar;
      }
      else
      {
        fprintf(stdout, "Error: problem parsing variable %s\n", 
        varName.c_str());
      }
    }
  }
}


std::map<std::string, OpenSim::Variable *> 
OpenSim::IOxml::Variables()
{
  return vars;
}
