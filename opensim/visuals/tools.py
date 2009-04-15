#===--- tools.py - Toolbars for Sugar OpenSim -----------------------------===#
#
# Copyright 2008 Bobby Powers
#
# This file is part of OpenSim.
# 
# OpenSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
#
#===-----------------------------------------------------------------------===#
#
# This file contains implementations of toolbars for the Model activity.
#
#===-----------------------------------------------------------------------===#

from gettext import gettext as _

import pygtk
pygtk.require("2.0")

import gtk
import gobject
import sys
import logging

from gtk import ToggleToolButton
from toolcombobox import ToolComboBox
from gtk import ToolButton

from gaphas import tool

from constants import *
import widgets


class ModelToolbar(gtk.Toolbar):
  '''Provides the toolbar containing the basic modeling functions'''

  def __init__(self):
    gtk.Toolbar.__init__(self)

    #Get our 4 buttons & add type attributes to them
    self.stock = ToggleToolButton()
    self.stock.set_label(_('Stock'))
    self.stock.set_tooltip_text(_('Create stocks'))
    self.stock.set_icon_name('opensim-stock')
    self.insert(self.stock, -1)
    self.stock.show()
    self.stock.type = STOCK

    self.flow = ToggleToolButton()
    self.flow.set_label(_('Flow'))
    self.flow.set_tooltip_text(_('Create flows'))
    self.flow.set_icon_name('opensim-flow')
    self.insert(self.flow, -1)
    self.flow.show()
    self.flow.type = FLOW

    self.variable = ToggleToolButton()
    self.variable.set_label(_('Variable'))
    self.variable.set_tooltip_text(_('Create auxiliary variables'))
    self.variable.set_icon_name('opensim-var')
    self.insert(self.variable, -1)
    self.variable.show()
    self.variable.type = VARIABLE

    self.influence = ToggleToolButton()
    self.influence.set_label(_('Influence'))
    self.stock.set_tooltip_text(_('Create influence arrows'))
    self.influence.set_icon_name('opensim-infl')
    self.insert(self.influence, -1)
    self.influence.show()
    self.influence.type = INFLUENCE

  def connect_toggled(self, callback):
    '''
    Connect our drawing tools to a callback function for the toggle signal.
    '''
    self.stock.connect('toggled', callback)
    self.flow.connect('toggled', callback)
    self.influence.connect('toggled', callback)
    self.variable.connect('toggled', callback)


class SimulateToolbar(gtk.Toolbar):
  '''Provides the toolbar containing the basic modeling functions'''

  def __init__(self):
    gtk.Toolbar.__init__(self)

    #Get our 4 buttons & add type attributes to them

    self.start_label = gtk.Label()
    self.start_label.set_text(_('Start time: '))
    self._add_widget(self.start_label)

    self.start = gtk.Entry()
    self.start.set_size_request(int(gtk.gdk.screen_width() / 12), -1)
    self.start.set_text(_('0'))
    self._add_widget(self.start)

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.end_label = gtk.Label()
    self.end_label.set_text(_('End time: '))
    self._add_widget(self.end_label)

    self.start = gtk.Entry()
    self.start.set_size_request(int(gtk.gdk.screen_width() / 12), -1)
    self.start.set_text(_('100'))
    self._add_widget(self.start)

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.timestep = ToolComboBox(label_text=_('Timestep:'))
    #self.timestep.combo.connect('changed', self.__share_changed_cb)
    self.timestep.combo.append_text(_('.125'))
    self.timestep.combo.append_text(_('.25'))
    self.timestep.combo.append_text(_('1'))
    self.timestep.combo.set_active(0)
    self.insert(self.timestep, -1)
    self.timestep.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.savestep = ToolComboBox(label_text=_('Savestep:'))
    #self.timestep.combo.connect('changed', self.__share_changed_cb)
    self.savestep.combo.append_text(_('1'))
    self.savestep.combo.append_text(_('.25'))
    self.savestep.combo.append_text(_('.125'))
    self.savestep.combo.set_active(0)
    self.insert(self.savestep, -1)
    self.savestep.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    separator.set_expand(True)
    self.insert(separator, -1)
    separator.show()

    self.run = ToolButton('media-playback-start')
    self.run.set_tooltip_text = _('Run Simulation')
    #self.run.props.accelerator = '<Ctrl>Q'
    #self.run.connect('clicked', self.__stop_clicked_cb)
    self.insert(self.run, -1)
    self.run.show()

  def _add_widget(self, widget, expand=False):
    tool_item = gtk.ToolItem()
    tool_item.set_expand(expand)

    tool_item.add(widget)
    widget.show()

    self.insert(tool_item, -1)
    tool_item.show()


class ViewToolbar(gtk.Toolbar):
  '''Provides the toolbar containing the basic modeling functions'''

  def __init__(self):
    gtk.Toolbar.__init__(self)

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    separator.set_expand(True)
    self.insert(separator, -1)
    separator.show()

    self.view_canvas = ToggleToolButton()
    self.view_canvas.set_label(_('View Model'))
    self.view_canvas.set_tooltip_text(_('View model diagram'))
    self.view_canvas.set_icon_name('opensim-canvas')
    self.insert(self.view_canvas, -1)
    self.view_canvas.set_active(True)
    self.view_canvas.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.view_graphs = ToggleToolButton()
    self.view_graphs.set_label(_('View results'))
    self.view_graphs.set_tooltip_text(_('View simulation graphs'))
    self.view_graphs.set_icon_name('opensim-graphs')
    self.insert(self.view_graphs, -1)
    self.view_graphs.show()
    
    # connect these after both exist to get rid of some of the errors 
    # we were getting on activity startup
    self.view_graphs.connect('toggled', self.__toggled)    
    self.view_canvas.connect('toggled', self.__toggled)
    
    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.view_var = ToolComboBox(label_text=_('View behavior of:'))
    #self.timestep.combo.connect('changed', self.__share_changed_cb)
    self.view_var.combo.append_text(_('rabbits'))
    self.view_var.combo.append_text(_('foxes'))
    self.view_var.combo.append_text(_('rabbit births'))
    self.view_var.combo.set_active(0)
    self.insert(self.view_var, -1)
    self.view_var.set_sensitive(False)
    self.view_var.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    separator.set_expand(True)
    self.insert(separator, -1)
    separator.show()

  def __toggled(self, widget):
    # fight infinite loops!
    if widget.get_active() is False:
      return

    if widget is self.view_graphs:
      self.view_canvas.set_active(False)
      self.view_var.set_sensitive(True)
    else:
      self.view_graphs.set_active(False)
      self.view_var.set_sensitive(False)


class EquationEditor(gtk.Dialog):
  '''
  Provides a pop-up window for editing the equations of variables
  '''

  def __init__(self, equation='', influences=None, **kwargs):
    gtk.Dialog.__init__(self, flags=gtk.DIALOG_MODAL|
                        gtk.DIALOG_DESTROY_WITH_PARENT, **kwargs)

    self.eqn_label = gtk.Label()
    self.eqn_label.set_text(_('Edit equation:'))
    self.vbox.pack_start(self.eqn_label, False, False)
    self.eqn_label.show()
    
    self.equation = gtk.Entry()
    self.equation.set_text(equation)
    self.equation.set_size_request(int(gtk.gdk.screen_width() / 4), -1)
    self.vbox.pack_start(self.equation, False, False)
    self.equation.show()

    if influences:
      infl_box = gtk.Frame()
      infl_box.set_label('Influences')
      infl_list = gtk.ListStore(gobject.TYPE_STRING)
      for var in influences:
        infl_list.append([var.var.props.name])
      self.infl_tree = gtk.TreeView(infl_list)
      infl_col = gtk.TreeViewColumn()
      self.infl_tree.append_column(infl_col)

      cell = gtk.CellRendererText()
      infl_col.pack_start(cell, True)
      infl_col.add_attribute(cell, 'text', 0)

      infl_box.add (self.infl_tree)
      self.infl_tree.show()
      self.vbox.pack_start(infl_box)
      infl_box.show()

    cancel = gtk.Button(stock=gtk.STOCK_CANCEL)
    self.add_action_widget(cancel, gtk.RESPONSE_CANCEL)
    cancel.show()
    ok = gtk.Button(stock=gtk.STOCK_OK)
    self.add_action_widget(ok, gtk.RESPONSE_OK)
    ok.show()


def edit_equation(var, influences=None):
  if var is None:
    raise ValueError
  logging.debug("showing equation editor for: %s" % var.props.name)
  eqn = var.props.equation
  if 0 and (eqn is None or eqn == ""):
    eqn = 'INTEG(<derivative>, <initial value>)'
  editor = EquationEditor(eqn, influences)
  result = editor.run()
  editor.hide()

  if result == gtk.RESPONSE_OK:
    logging.debug("okay, got an equation:")
    logging.debug("\t'%s'" % editor.equation.get_text())
    var.props.equation = editor.equation.get_text()
  else:
    logging.debug('oh well, canceled editor or something.')


class PlacementTool(tool.Tool):

  def __init__(self, model, obj_type):
    self._model = model
    self._handle_tool = tool.HandleTool()
    self._handle_index = 2
    self._new_type = obj_type
    self._new_item = None
    self._grabbed_handle = None

  handle_tool = property(lambda s: s._handle_tool, doc="Handle tool")
  handle_index = property(lambda s: s._handle_index,
                            doc="Index of handle to be used by handle_tool")
  new_item = property(lambda s: s._new_item, doc="The newly created item")

  def on_button_press(self, context, event):
    view = context.view
    canvas = view.canvas
    new_item = self._create_item(context, (event.x, event.y))
    # Enforce matrix update, as a good matrix is required for the handle
    # positioning:
    canvas.get_matrix_i2c(new_item, calculate=True)

    self._new_item = new_item
    view.focused_item = new_item

    h = new_item.handles()[self._handle_index]
    if h.movable:
      self._handle_tool.grab_handle(new_item, h)
      self._grabbed_handle = h
      context.grab()
    return True

  def _create_item(self, context, pos):
    if self._new_type == 'stock':
      item = self._model.new_stock(*pos)
    elif self._new_type == 'variable':
      item = self._model.new_variable(*pos)
    else:
      raise ValueError, 'bad new type: "%s"' % self._new_type
    return item

  def on_button_release(self, context, event):
    context.ungrab()
    if self._grabbed_handle:
      self._handle_tool.on_button_release(context, event)
      self._grabbed_handle = None
    self._new_item = None
    return True

  def on_motion_notify(self, context, event):
    if self._grabbed_handle:
      return self._handle_tool.on_motion_notify(context, event)
    else:
      # act as if the event is handled if we have a new item
      return bool(self._new_item)

