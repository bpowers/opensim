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
from sugar.graphics.toggletoolbutton import ToggleToolButton
from sugar.graphics.toolcombobox import ToolComboBox
from sugar.graphics.toolbutton import ToolButton
from sugar.graphics.icon import Icon
import logging

from constants import *
import widgets


class ModelToolbar(gtk.Toolbar):
  '''Provides the toolbar containing the basic modeling functions'''

  def __init__(self):
    gtk.Toolbar.__init__(self)

    #Get our 4 buttons & add type attributes to them
    self.stock = ToggleToolButton("opensim-stock")
    self.stock.set_tooltip(_('Create Stocks'))
    self.insert(self.stock, -1)
    self.stock.show()
    self.stock.type = STOCK

    self.flow = ToggleToolButton("opensim-flow")
    self.flow.set_tooltip(_('Create Flows'))
    self.insert(self.flow, -1)
    self.flow.show()
    self.flow.type = FLOW    

    self.variable = ToggleToolButton("opensim-var")
    self.variable.set_tooltip(_('Create Variables'))
    self.insert(self.variable, -1)
    self.variable.show()
    self.variable.type = VARIABLE

    self.influence = ToggleToolButton("opensim-infl")
    self.influence.set_tooltip(_('Create Influence Arrows'))
    self.insert(self.influence, -1)
    self.influence.show()
    self.influence.type = INFLUENCE


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
    self.start.set_text(_('2008'))
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
    self.start.set_text(_('2050'))
    self._add_widget(self.start)

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.timestep = ToolComboBox(label_text=_('Timestep:'))
    #self.timestep.combo.connect('changed', self.__share_changed_cb)
    self.timestep.combo.append_item(.125, _('.125'))
    self.timestep.combo.append_item(.25, _('.25'))
    self.timestep.combo.append_item(1, _('1'))
    self.timestep.combo.set_active(0)
    self.insert(self.timestep, -1)
    self.timestep.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.savestep = ToolComboBox(label_text=_('Savestep:'))
    #self.timestep.combo.connect('changed', self.__share_changed_cb)
    self.savestep.combo.append_item(1, _('1'))
    self.savestep.combo.append_item(.25, _('.25'))
    self.savestep.combo.append_item(.125, _('.125'))
    self.savestep.combo.set_active(0)
    self.insert(self.savestep, -1)
    self.savestep.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    separator.set_expand(True)
    self.insert(separator, -1)
    separator.show()

    self.run = ToolButton('media-playback-start', tooltip=_('Run Simulation'))
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

    self.view_canvas = ToggleToolButton('opensim-canvas')
    self.view_canvas.set_tooltip(_('View model diagram'))
    self.view_canvas.connect('toggled', self.__toggled)
    self.insert(self.view_canvas, -1)
    self.view_canvas.set_active(True)
    self.view_canvas.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.view_graphs = ToggleToolButton('opensim-graphs')
    self.view_graphs.set_tooltip(_('View simulation graphs'))
    #self.run.props.accelerator = '<Ctrl>Q'
    self.view_graphs.connect('toggled', self.__toggled)
    self.insert(self.view_graphs, -1)
    self.view_graphs.show()

    separator = gtk.SeparatorToolItem()
    separator.props.draw = False
    self.insert(separator, -1)
    separator.show()

    self.view_var = ToolComboBox(label_text=_('View behavior of:'))
    #self.timestep.combo.connect('changed', self.__share_changed_cb)
    self.view_var.combo.append_item(1, _('rabbits'))
    self.view_var.combo.append_item(2, _('foxes'))
    self.view_var.combo.append_item(3, _('rabbit births'))
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
  '''Provides a pop-up window for editing the equations of variables'''

  def __init__(self, equation='', **kwargs):
    gtk.Dialog.__init__(self, flags=gtk.DIALOG_MODAL|
                        gtk.DIALOG_DESTROY_WITH_PARENT, **kwargs)

    self.equation = gtk.Entry()
    self.equation.set_text(equation)
    self.equation.set_size_request(int(gtk.gdk.screen_width() / 4), -1)
    self.vbox.pack_start(self.equation, False, False, 10)
    self.equation.show()

    cancel = gtk.Button(stock=gtk.STOCK_CANCEL)
    self.add_action_widget(cancel, gtk.RESPONSE_CANCEL)
    cancel.show()
    ok = gtk.Button(stock=gtk.STOCK_OK)
    self.add_action_widget(ok, gtk.RESPONSE_OK)
    ok.show()


class LineControl:
  '''
  This class keeps track of things related to the process of
  adding new lines to the canvas.
  '''
  def __init__(self):
    # if we've got a line we're making, we want to attach the
    # motion callback to it.  When we're done moving the line, 
    # detach the callback.  keep track of the callback id here.
    self.cb_id = None
    self.line = None
    self._canvas = None


  def set_canvas(self, canvas):
    self._canvas = canvas


  def cleanup(self, item):
    if item is not self.line:
      return

    logging.debug("LineControl: cleaning up line junk.")

    if self.cb_id and self._canvas:
      logging.debug("LineControl: disconnecting callback")
      self._canvas.goocanvas.get_root_item().disconnect(self.cb_id)
      self.cb_id = None
    self.line = None


  def new_flow(self, flow_from):
    if not flow_from:
      logging.error("LineControl: no widget as source.")

    root = self._canvas.goocanvas.get_root_item()

    new_var = widgets.FlowItem(flow_from=flow_from,
                               parent=root, can_focus=True)
    self.cb_id = root.connect("motion_notify_event", 
                              new_var.on_motion_notify)

    flow_from.raise_(None)
    self.line = new_var
    self._canvas.display_vars.append(new_var)


  def new_link(self, link_from):
    if not link_from:
      logging.error("LineControl: no widget as source.")

    root = self._canvas.goocanvas.get_root_item()

    new_var = widgets.LinkItem(flow_from=link_from,
                               parent=root, can_focus=True)
    self.cb_id = root.connect("motion_notify_event", 
                              new_var.on_motion_notify)

    link_from.raise_(None)
    self.line = new_var
    self._canvas.display_vars.append(new_var)

  
  def end_flow(self, flow_to):
    if not self.cb_id or not self._canvas:
      logging.error("LineControl: something is screwey finishing line.")
    
    self.line.set_flow_to(flow_to)
    flow_to.raise_(None)

    self.cleanup(self.line)

