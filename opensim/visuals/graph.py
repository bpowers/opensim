#===--- graph.py - FortranLive result viewer ------------------------------===#
#
# Copyright 2008 Bobby Powers
#
# This file is part of OpenSim
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
# This file contains some initialization needed for the Python modules
#
#===-----------------------------------------------------------------------===#

import pygtk
pygtk.require ('2.0')
import gtk

import os, math
import cairo
import CairoPlot


class GraphViewer(gtk.DrawingArea):

  def __init__(self):
    gtk.DrawingArea.__init__(self)
    self.connect("expose_event", self.expose)
    self.y_axis = None
    self.data = None
    self.all_data = None
    self.series = None

    
  def _set_y_axis(self, series):
    '''
    Sets the y-axis _labels_ for our graph
    '''

    self.y_axis = []
    for i in series:
      self.y_axis.append(str(int(i)))

  
  def set_data(self, data_dict):
    self.all_data = data_dict
    self._set_y_axis(self)


  def set_series(self, series_name):
    self.series = series_name
    if self.series and self.data.has_key(self.series_name):
      self.data = self.data[self.series_name]

    self.window.invalidate_rect(self.allocation, True)


  def expose(self, widget, event):
    self.context = widget.window.cairo_create()
        
    # set a clip region for the expose event
    self.context.rectangle(event.area.x, event.area.y,
                           event.area.width, event.area.height)
    self.context.clip()
        
    self.draw(self.context)
        
    return False


  def draw(self, context):
    if not self.data: 
      return
    size = self.allocation
    surface = cairo.ImageSurface (cairo.FORMAT_ARGB32, size.width, size.height)
    data_dic = {'current': self.data}
    CairoPlot.dot_line_plot(surface, data_dic, size.width, size.height-10, h_labels = self.y_axis, axis = True, grid = True)
    context.set_source_surface(surface, 0, 0)
    context.paint()



def dictionary_for_csv(file_path):
  
  if not os.path.isfile(file_path):
    print "error: unable to find csv '%s'" % file_path
    return None

  csv_in = open(file_path, 'r')
  
  if not csv_in:
    print "error: couldn't open csv '%s' for reading" % file_path
    return None

  csv_lines = csv_in.readlines()
  
  if not csv_lines or len(csv_lines) is 0:
    print "error: empty csv or something"
    return None
  
  headers = csv_lines[0].strip().split(',')
  cols = len(headers)
  data = [[] for i in range(cols)]

  for line in csv_lines[1:]:
    row = line.split(',')
    for i in range (cols):
      data[i].append(float(row[i]))

  dict_out = {}
  for i in range(cols):
    dict_out[headers[i]] = data[i]

  csv_in.close()
  return dict_out

