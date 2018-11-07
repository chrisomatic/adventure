import pandas as pd
import numpy as np
import os
import ctypes

import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QKeyEvent, QPainter,QImage, QPen, QIcon, QPixmap, QColor, QBrush, QCursor, QFont
from PyQt5.QtCore import Qt, QPoint, QPointF, QSize, QEvent

import editor

# TODO
# 1) add align to grid check box for drawing objects
# 2) add check box for showing/hiding the objects


class MyWidget(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)


        self.font = QFont('Arial', 9)
        self.setFont(self.font)
        QToolTip.setFont(self.font)

        self.setWindowTitle('Excel Grid Editor')

        self.ico = r"X:\DEPO\SOFTWARE\SUPPORT\excel.ico"
        if os.path.isfile(self.ico):
            self.setWindowIcon(QIcon(self.ico))

        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID("adventure_map_editor")

        self.root = os.path.dirname(os.path.abspath(__file__))  + "\\"

        self.housekeeping()

        self.editor = editor.Editor(self)
        self.editor.setMinimumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMaximumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMinimumHeight(self.editor.board_height*self.editor.tile_size_zoom)
        self.editor.setMaximumHeight(self.editor.board_height*self.editor.tile_size_zoom)
        self.editor.ts_path = self.ts_path
        
        self.scroll = self.scroll_area(self.editor)
        self.scroll.verticalScrollBar().valueChanged.connect(self.repaint)
        self.scroll.horizontalScrollBar().valueChanged.connect(self.repaint)

        self.build_ts_combo()

        self.tool_combo = QComboBox(self)
        self.tool_list = ['Pen','Rectangle','Rectangle Fill','Fill','Copy Range','Objects']
        self.tool_combo.addItems(self.tool_list)
        self.tool_combo.activated[str].connect(self.change_tool)
        self.tool_combo.setToolTip("Ctrl + D")

        self.ptool = ""

        self.sld = QSlider(Qt.Horizontal, self)
        self.sld.setMinimum(1)
        self.sld.setMaximum(64)
        self.sld.setMaximumWidth(200)
        self.sld.valueChanged.connect(self.change_size)
        self.sld.setToolTip("Change the brush size for the pen tool.")

        self.sld_zoom = QSlider(Qt.Horizontal, self)
        self.zoom_values = [1/16,1/8,.25,.5,1,2,4,8,16]
        self.sld_zoom.setMinimum(1)
        self.sld_zoom.setMaximum(len(self.zoom_values))
        self.sld_zoom.setMaximumWidth(100)
        self.sld_zoom.valueChanged.connect(self.change_zoom)

        self.save_btn = QPushButton('Save', self)
        self.save_btn.clicked.connect(self.save_map)
        self.save_btn.resize(self.save_btn.sizeHint())
        self.save_btn.setToolTip("Ctrl + S")

        self.save2_btn = QPushButton('Save PNG', self)
        self.save2_btn.clicked.connect(self.save_png)
        self.save2_btn.resize(self.save2_btn.sizeHint())
        self.save2_btn.setToolTip("Save the map as a png.")

        self.load_btn = QPushButton('Load', self)
        self.load_btn.clicked.connect(self.load_map)
        self.load_btn.resize(self.load_btn.sizeHint())
        self.load_btn.setToolTip("Ctrl + O")

        self.clear_btn = QPushButton('Clear', self)
        self.clear_btn.clicked.connect(self.clear_map)
        self.clear_btn.resize(self.clear_btn.sizeHint())
        self.clear_btn.setToolTip("Clear the current map.")

        self.undo_btn = QPushButton('Undo', self)
        self.undo_btn.clicked.connect(self.undo)
        self.undo_btn.resize(self.undo_btn.sizeHint())
        self.undo_btn.setToolTip("Ctrl + Z")

        self.draw_over_chk = QCheckBox("Draw Over Tiles",self)
        self.draw_over_chk.setChecked(True)
        self.draw_over_chk.setToolTip("Unselect this to only draw on tiles that are empty.")
        self.draw_over_chk.installEventFilter(self)
        
        self.draw_grid_chk = QCheckBox("Show Grid",self)
        self.draw_grid_chk.setChecked(True)
        self.draw_grid_chk.setToolTip("Unselect this to hide the grid.")
        self.draw_grid_chk.installEventFilter(self)

        self.draw_objs_chk = QCheckBox("Show Objects",self)
        self.draw_objs_chk.setChecked(True)
        self.draw_objs_chk.setToolTip("Unselect this to hide objects.")
        self.draw_objs_chk.installEventFilter(self)

        self.align_grid_chk = QCheckBox("Align To Grid",self)
        self.align_grid_chk.setChecked(True)
        self.align_grid_chk.setToolTip("Select this if you wish to draw objects aligned to the grid.")
        self.align_grid_chk.installEventFilter(self)


        self.lbl_pen_size = QLabel('Pen Size: 1', self)

        self.lbl_zoom = QLabel('Zoom: ', self)

        self.line_edit = QLineEdit(self.ad_path,self)
        self.line_edit.setToolTip("Adventure Path")

        self.save_path_btn = QPushButton('Save', self)
        self.save_path_btn.clicked.connect(self.save_path)
        self.save_path_btn.resize(self.save_path_btn.sizeHint())
        self.save_path_btn.setToolTip("Save Adventure path")


        self.list_tiles("tiles")

        self.grid = QGridLayout()
        self.grid.setSpacing(10)
        self.grid.addWidget(self.scroll, 0, 0, 11, 1)
        self.grid.addWidget(self.tool_combo, 0, 1)
        self.grid.addWidget(self.lbl_pen_size, 1, 1)
        self.grid.addWidget(self.sld, 2, 1)
        self.grid.addWidget(self.draw_over_chk, 3, 1)
        self.grid.addWidget(self.ts_combo, 4, 1)
        self.grid.addWidget(self.qlist, 5, 1, 6, 1) # also change in load_tileset()

        self.grid.addWidget(self.line_edit,11,0,1,2)
        self.grid.addWidget(self.save_path_btn,11,2)

        self.grid.addWidget(self.save_btn, 0, 2)
        self.grid.addWidget(self.load_btn, 1, 2)
        self.grid.addWidget(self.clear_btn, 3, 2)
        self.grid.addWidget(self.undo_btn, 4, 2)
        self.grid.addWidget(self.save2_btn, 5, 2)
        self.grid.addWidget(self.draw_grid_chk, 6, 2)
        self.grid.addWidget(self.draw_objs_chk, 7, 2)
        self.grid.addWidget(self.align_grid_chk, 8, 2)
        self.grid.addWidget(self.lbl_zoom, 9, 2)
        self.grid.addWidget(self.sld_zoom, 10, 2)
        
        
        self.setLayout(self.grid)

        self.sld_zoom.setValue(self.zoom_values.index(1)+1)

        self.setGeometry(300, 300, 1000, 500)
        
        self.center()

        self.installEventFilter(self)

    def save_path(self):
        with open(self.adpath_path,'w') as f:
            f.write(self.line_edit.text())
        
        with open(self.adpath_path,'r') as f:
            self.ad_path = f.read().strip()
            if len(self.ad_path) >= 1:
                if self.ad_path[-1] != "\\":
                    self.ad_path += "\\"
        
        if not os.path.isdir(self.ad_path):
            self.ad_path = self.root + ""

        self.ts_path = self.ad_path + "data\\data\\tilesets\\"
        if not os.path.isdir(self.ts_path):
            self.ts_path = self.ad_path + "data\\tilesets\\"
            if not os.path.isdir(self.ts_path):
                self.ts_path = self.ad_path + ""
        self.editor.ts_path = self.ts_path

        self.ob_path = self.ad_path + "data\\data\\objects\\"
        if not os.path.isdir(self.ob_path):
            self.ob_path = self.ad_path + "data\\objects\\"
            if not os.path.isdir(self.ob_path):
                self.ob_path = self.ad_path + ""


        self.tilesets = [x for x in os.listdir(self.ts_path) if ".tileset.png" in x]
        self.object_lst = [x for x in os.listdir(self.ob_path) if ".png" in x]


        # self.editor.tiles = {}
        # self.editor.tiles_actual = {}
        self.editor.tile_set_name = ""
        self.build_ts_combo()
        self.grid.addWidget(self.ts_combo, 4, 1)
        self.setLayout(self.grid)

        if self.editor.tool == "objects":
            self.editor.build_objects(self.ob_path,self.object_lst)
            self.load_tileset("objects")
        else:
            self.load_tileset("")
       
        

   
    def build_ts_combo(self):
        self.ts_combo = QComboBox(self)
        for i in self.tilesets:
            self.ts_combo.addItem(i)

        t = "terrain.tileset.png"
        if t in self.tilesets:
            self.editor.build_tiles(self.ts_path + t)
            index = self.ts_combo.findText(t, Qt.MatchFixedString)
            self.ts_combo.setCurrentIndex(index)
            self.editor.tile_set_name = t
        elif len(self.tilesets) > 0:
            self.editor.build_tiles(self.ts_path + self.tilesets[0])
            index = self.ts_combo.findText(self.tilesets[0], Qt.MatchFixedString)
            self.ts_combo.setCurrentIndex(index)
            self.editor.tile_set_name = self.tilesets[0]

        self.ts_combo.activated[str].connect(self.load_tileset)
        self.ts_combo.setToolTip("Ctrl + T")


    def change_zoom(self):
        value = self.sld_zoom.value()
        zoom = 1/self.zoom_values[value-1]

        self.editor.zoom_ratio = zoom
        self.editor.tile_size_zoom = int(self.editor.tile_size * zoom)
        self.editor.setMinimumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMaximumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMinimumHeight(self.editor.board_height*self.editor.tile_size_zoom)
        self.editor.setMaximumHeight(self.editor.board_height*self.editor.tile_size_zoom)

        self.scroll = self.scroll_area(self.editor)
        self.scroll.verticalScrollBar().valueChanged.connect(self.repaint)
        self.scroll.horizontalScrollBar().valueChanged.connect(self.repaint)

        self.grid.addWidget(self.scroll, 0, 0, 11, 1)
        self.setLayout(self.grid)

        self.editor.build_tiles(self.ts_path + self.editor.tile_set_name)

        # self.lbl_zoom.setText("Zoom: x" + str(int(self.editor.tile_size*zoom)))
        if zoom == int(zoom):
            l = str(int(zoom))
        elif zoom == 1/2:
            l = "1/2"
        elif zoom == 1/4:
            l = "1/4"
        elif zoom == 1/8:
            l = "1/8"
        elif zoom == 1/16:
            l = "1/16"
        self.lbl_zoom.setText("Zoom: x" + l)

        self.repaint()
        
    
    def housekeeping(self):
        from getpass import getuser
        self.user = getuser()

        self.appd = "C:\\Users\\"+ self.user +"\\AppData\\Local\\map_editor\\"
        if not(os.path.isdir(self.appd)):
            os.makedirs(self.appd)

        self.board_path = self.root + ""
        self.ad_path = self.root + ""


        # board path ------- where you save the board files to
        self.path_path = self.appd + "PATH"
        if not os.path.isfile(self.path_path):
            with open(self.path_path,'w') as f:
                f.write(self.board_path)
        
        with open(self.path_path,'r') as f:
            self.board_path = f.read().strip()
        
        if not os.path.isdir(self.board_path):
            self.board_path = self.root + ""

        # adventure path ------- where the pngs are located
        self.adpath_path = self.appd + "ADVENTURE"
        if not os.path.isfile(self.adpath_path):
            with open(self.adpath_path,'w') as f:
                f.write(self.ad_path)
        
        with open(self.adpath_path,'r') as f:
            self.ad_path = f.read().strip()
            if len(self.ad_path) >= 1:
                if self.ad_path[-1] != "\\":
                    self.ad_path += "\\"

        
        if not os.path.isdir(self.ad_path):
            self.ad_path = self.root + ""

        self.ts_path = self.ad_path + "data\\data\\tilesets\\"
        if not os.path.isdir(self.ts_path):
            self.ts_path = self.ad_path + "data\\tilesets\\"
            if not os.path.isdir(self.ts_path):
                self.ts_path = self.ad_path + ""

        self.ob_path = self.ad_path + "data\\data\\objects\\"
        if not os.path.isdir(self.ob_path):
            self.ob_path = self.ad_path + "data\\objects\\"
            if not os.path.isdir(self.ob_path):
                self.ob_path = self.ad_path + ""

        self.tilesets = [x for x in os.listdir(self.ts_path) if ".tileset.png" in x]
        self.object_lst = [x for x in os.listdir(self.ob_path) if ".png" in x]



    def resizeEvent(self, event):
        self.repaint()
        # self.editor.update()

    def eventFilter(self,source,event):

        if source is self.draw_over_chk:
            self.editor.draw_over = self.draw_over_chk.isChecked()

        elif source is self.draw_objs_chk:
            self.editor.draw_objs = self.draw_objs_chk.isChecked()
            self.editor.update()

        elif source is self.align_grid_chk:
            self.editor.align_objs = self.align_grid_chk.isChecked()

        elif source is self.draw_grid_chk:
            self.editor.grid = self.draw_grid_chk.isChecked()
            self.editor.update()

        elif event.type() == QEvent.KeyPress:

            modifiers = QApplication.keyboardModifiers()

            # ctrl + z
            if modifiers == Qt.ControlModifier and event.key() == Qt.Key_Z:
                self.undo()
            
            # ctrl + d
            if modifiers == Qt.ControlModifier and event.key() == Qt.Key_D:

                ind = self.tool_combo.currentIndex()
                new_ind = (ind + 1) % len(self.tool_list)
                tool = self.tool_list[new_ind]

                self.tool_combo.setCurrentIndex(new_ind)
                self.change_tool(tool.lower())

            # ctrl + t
            if modifiers == Qt.ControlModifier and event.key() == Qt.Key_T:

                items = []
                for index in range(self.qlist.count()):
                    items.append(self.qlist.item(index))

                new_ind = self.editor.tile_index + 1
                if new_ind >= len(self.qlist_text)-1:
                    new_ind = -1

                self.editor.tile_index = new_ind # (self.editor.tile_index + 1) % len(self.qlist_text)
                items[new_ind+1].setSelected(True)

            # ctrl + s
            if modifiers == Qt.ControlModifier and event.key() == Qt.Key_S:
                self.save_map()
            
            # ctrl + o
            if modifiers == Qt.ControlModifier and event.key() == Qt.Key_O:
                self.load_map()
            
            # ctrl + f
            if modifiers == Qt.ControlModifier and event.key() == Qt.Key_F:
                # self.load_map()
                x = FindAndReplace()
                x.show()
    

        return 0


    def clear_map(self):
        for j in range(0,self.editor.board_height):
            for i in range(0,self.editor.board_width):
                self.editor.board[j][i].tile_index = -1
                self.editor.board[j][i].tile_set_name = "-1"
        self.repaint()
        self.editor.update()

    def save_map(self):

        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getSaveFileName(self,"Save",self.board_path,"Board Files (*.board);;All Files (*)", options=options)
        if not fileName:
            return 
        
        fileName = fileName.replace("/","\\")

        self.board_path = "\\".join(fileName.split("\\")[:-1])
        with open(self.path_path,'w') as f:
            f.write(self.board_path)

        map_string = b""
        tile_set_number = 0
        tile_sets = {"255":255}

        for i in self.editor.board:

            lst = []
            for j in i:
                ti = j.tile_index
                tsn = j.tile_set_name
                if tsn == "-1":
                    tsn = "255"
                if ti == -1:
                    ti = 255
                    tsn = "255"

                if not tsn in tile_sets.keys():
                    value = tile_set_number+0
                    tile_set_number += 1
                    tile_sets[tsn] = value

                lst.append(bytes([int(tile_sets[tsn])])  + bytes([ti]))

            map_string += b"".join(lst)



            other = ":board_info" + "\n"
            other += "width_in_tiles=" + str(self.editor.board_width) + "\n"
            other += "height_in_tiles=" + str(self.editor.board_height) + "\n"
            # other += "\n"

            objs = {}
            obj_string = ""
            obj_number = 0
            for pa in self.editor.painted_objects:
                if not pa.png in objs.keys():
                    value = obj_number+0
                    obj_number += 1
                    objs[pa.png] = value
                obj_string += str(objs[pa.png]) + "," + str(int(pa.x)) + "," + str(int(pa.y)) + "\n"


            other += ":object_info" + "\n"
            for k in objs.keys():
                other += str(objs[k]) + "=" + k + "\n"
            # other += "\n"

            other += ":object_data" + "\n"
            other += obj_string
            # other += "\n"

            other += ":tileset_info" + "\n"
            for k in tile_sets.keys():
                if k != "255":
                    other += str(tile_sets[k]) + "=" + k + "\n"
            # other += "\n"

            other += ":data" + "\n"

        with open(fileName,'wb') as f:
            f.write(other.encode())
            f.write(map_string)
            f.write(b"\n:data_end")


    def load_map(self):

        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getOpenFileName(self,"Save",self.board_path,"Board Files (*.board);;All Files (*)", options=options)
        if not fileName:
            return 

        fileName = fileName.replace("/","\\")

        self.board_path = "\\".join(fileName.split("\\")[:-1])
        with open(self.path_path,'w') as f:
            f.write(self.board_path)
        

        with open(fileName,'rb') as f:
            l = f.read()

        lines0 = "".join([chr(x) for x in l]).split("\n")
        d0 = lines0.index(":data")
        d1 = lines0.index(":data_end")

        data_chars = "\n".join(lines0[d0+1:d1])
        dvalues = [ord(x) for x in data_chars]

        lines = lines0[:d0] + lines0[d1+1:]
        lines = [x.strip().lower() for x in lines if x.strip() != ""]

        w = [x.split("width_in_tiles=")[-1] for x in lines if "width_in_tiles=" in x][0]
        h = [x.split("height_in_tiles=")[-1] for x in lines if "height_in_tiles=" in x][0]
        

        ti = lines.index(":tileset_info")
        tile_sets = {"255":255}
        tile_sets_missing = []
        

        for ind in range(ti+1,len(lines)):
            l = lines[ind]
            if ":" in l:
                break
            if "tileset_dir=" in l:
                continue
            v = int(l.split("=")[0])
            k = l.split("=")[1]
            if not k in tile_sets.keys():
                tile_sets[k] = v

            if os.path.isfile(self.ts_path + k):
                self.editor.build_tiles(self.ts_path + k)
            else:
                tile_sets_missing.append(k)

        self.editor.board_width  = int(w)
        self.editor.board_height = int(h)
        self.editor.board = [[editor.Tile() for i in range(self.editor.board_width)] for j in range(self.editor.board_height)]


        yind = 0
        coords = []
        for i in range(len(dvalues)-1):
            if i % 2 == 1:
                continue
            
            xind = int(i/2) % self.editor.board_width
            if xind == 0 and i != 0:
                yind += 1
            coords.append((xind,yind))
            
            ts = dvalues[i]
            tsn = [x for x in tile_sets.keys() if tile_sets[x] == ts][0] # tile set name
            tile_index = dvalues[i+1]

            if tsn == "255" or tsn in tile_sets_missing:
                tile_index = -1
                tsn = "-1"
            
            self.editor.board[yind][xind].tile_index = tile_index
            self.editor.board[yind][xind].tile_set_name = tsn


        self.editor.painted_objects = []
        self.object_lst = [x for x in os.listdir(self.ob_path) if ".png" in x]
        self.editor.build_objects(self.ob_path,self.object_lst)
        if ":object_info" in lines:
            ti = lines.index(":object_info")
            objs = {}
            objs_missing = []
            for ind in range(ti+1,len(lines)):
                l = lines[ind]
                if ":" in l:
                    break
                v = int(l.split("=")[0])
                k = l.split("=")[1]
                if not k in objs.keys():
                    objs[k] = v

                if not os.path.isfile(self.ob_path + k):
                    objs_missing.append(k)
                

            ti = lines.index(":object_data")
            for ind in range(ti+1,len(lines)):
                l = lines[ind]
                if ":" in l:
                    break
                ls = l.split(",")
                if len(ls) != 3:
                    continue
                v = int(ls[0])
                x = int(ls[1])
                y = int(ls[2])
                png = [x for x in objs.keys() if objs[x] == v][0]
                obj = editor.Object()
                obj.x = x
                obj.y = y
                obj.png = png
                self.editor.painted_objects.append(obj)

        self.repaint()
 


    def change_tool(self,text):
        self.ptool = self.editor.tool + ""
        self.editor.tool = text.lower()
        
        self.editor.copied_range = False
        self.editor.copy_moved = False
        self.editor.copying = False

        if text.lower() == "objects":
            self.object_lst = [x for x in os.listdir(self.ob_path) if ".png" in x]
            self.editor.build_objects(self.ob_path,self.object_lst)
            self.load_tileset("objects")
        else:
            if self.ptool == "objects":
                self.load_tileset("")



    def change_size(self):
        self.editor.bsize = max(1,int(self.sld.value()))
        self.lbl_pen_size.setText("Pen Size: " + str(self.editor.bsize))



    def repaint(self):
        
        self.v_lbound = self.scroll.verticalScrollBar().value()
        self.v_ubound = self.v_lbound + self.scroll.viewport().height()

        self.h_lbound = self.scroll.horizontalScrollBar().value()
        self.h_ubound = self.h_lbound + self.scroll.viewport().width()

        self.editor.v_lbound = self.v_lbound
        self.editor.v_ubound = self.v_ubound
        self.editor.h_lbound = self.h_lbound
        self.editor.h_ubound = self.h_ubound

        self.editor.update()

        # print("vertical:",self.v_lbound,self.v_ubound)
        # print("horizontal:",self.h_lbound,self.h_ubound)

        
    def load_tileset(self,text):

        if self.editor.tool != "objects":
            if os.path.isfile(self.ts_path + self.ts_combo.currentText()):
                self.editor.build_tiles(self.ts_path + self.ts_combo.currentText())
            self.editor.tile_index = 0
            # self.qlist.setParent(None)
            self.list_tiles("tiles")
        else:
            self.list_tiles("objects")
            self.editor.object_name = "eraser"

        self.grid.addWidget(self.qlist, 5, 1, 6, 1)
        self.setLayout(self.grid)


    def eraser(self):
        self.eraser_item = QListWidgetItem()
        self.eraser_item.setText("Eraser")
        if os.path.isfile("eraser.png"):
            self.eraser_item.setIcon(QIcon("eraser.png"))


    def list_tiles(self,string):

        self.qlist = QListWidget(self)
        
        self.qlist.setMinimumWidth(150)
        self.qlist.setMaximumWidth(200)

        self.qlist.clear()
        self.qlist_text = ["Eraser"]

        self.eraser()
        self.qlist.addItem(self.eraser_item)     

        if string == "tiles":
            if self.editor.tile_set_name in self.editor.tiles.keys():
                tiles = self.editor.tiles[self.editor.tile_set_name]
                tiles_actual = self.editor.tiles_actual[self.editor.tile_set_name]
            else:
                tiles = []
                tiles_actual = []

            for i in range(len(tiles)):

                item = QListWidgetItem()
                item.setText('Tile ' + str(i))
                item.setIcon(QIcon(QPixmap.fromImage(tiles_actual[i])))
            
                colors = []
                for ii in range(tiles[i].width()):
                    for jj in range(tiles[i].height()):
                        colors.append(QColor(tiles[i].pixel(ii,jj)).getRgbF())
    
                if list(set(colors)) != [(1.0, 0.0, 1.0, 1.0)]:
                    self.qlist.addItem(item)
                    self.qlist_text += ["Tile " + str(i)]

        elif string == "objects":

            for k in self.editor.objects.keys():

                item = QListWidgetItem()
                item.setText(k)
                item.setIcon(QIcon(QPixmap.fromImage(self.editor.objects[k])))

                self.qlist.addItem(item)
                self.qlist_text += [k]
            
        self.qlist.itemClicked.connect(self.qlist_clicked)
    
    def qlist_clicked(self,curr):
    
        self.qlist.selectedItems()
        itext = curr.text()

        if self.editor.tool == "objects":

            self.editor.tile_index = -1
            self.editor.object_name = itext
        else:

            if itext == "Eraser":
                ind = -1
            else:
                ind = int(itext.split("Tile ")[-1])
            self.editor.tile_index = ind

    def save_png(self):

        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getSaveFileName(self,"Save",self.root,"png file (*.png);;All Files (*)", options=options)
        if not fileName:
            return 

        self.image = QImage(self.editor.board_width * self.editor.tile_size_zoom, self.editor.board_height * self.editor.tile_size_zoom, QImage.Format_RGB32)
        painter = QPainter(self)
        painter.begin(self.image)
        for i in range(0,self.editor.board_width):
            for j in range(0,self.editor.board_height):
                if(self.editor.board[j][i].tile_index > -1):
                    s = self.editor.board[j][i]
                    painter.drawImage(i*self.editor.tile_size_zoom,j*self.editor.tile_size_zoom,self.editor.tiles[s.tile_set_name][s.tile_index])

        painter.end()
        self.image.save(fileName)
    
    def undo(self):
        p = [x for x in self.editor.board_prev]
        c = [x for x in self.editor.board]
        self.editor.board = p
        self.editor.board_prev = c
        self.repaint()
        # self.editor.update()


    def scroll_area(self,_obj):
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded) 
        scroll.setWidget(_obj)
        return scroll

    def center(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())





class FindAndReplace(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()        
        
    def initUI(self):

        self.setFont(w.font)
        self.setWindowTitle('Find and Replace Tiles') 

        self.ico = w.ico
        if os.path.isfile(self.ico):
            self.setWindowIcon(QIcon(self.ico))

        # self.tiles = {}
        self.tiles_actual = {}
        self.tile_set_name = ""
        self.qlist1_text = []
        self.tile_index1 = -1

        # self.tiles2 = {}
        self.tiles_actual2 = {}
        self.tile_set_name2 = ""
        self.qlist2_text = []
        self.tile_index2 = -1

        self.lbl1 = QLabel('Find', self)
        self.lbl2 = QLabel('Replace', self)


        self.ts_combo1 = QComboBox(self)
        for i in w.tilesets:
            self.ts_combo1.addItem(i)
        self.ts_combo1.activated[str].connect(lambda x: self.build_qlist1(self.qlist1,2,0))


        self.qlist1 = QListWidget(self)
        self.qlist1.setMinimumWidth(150)
        self.qlist1.setMaximumWidth(200)
        

        self.ts_combo2 = QComboBox(self)
        for i in w.tilesets:
            self.ts_combo2.addItem(i)
        self.ts_combo2.activated[str].connect(lambda x: self.build_qlist1(self.qlist2,2,1))

        self.qlist2 = QListWidget(self)
        self.qlist2.setMinimumWidth(150)
        self.qlist2.setMaximumWidth(200)



        self.btn = QPushButton('Execute', self)
        self.btn.clicked.connect(self.execute)
        # self.btn.resize(self.btn.sizeHint())
        self.btn.setFixedSize(50,23)

        self.grid = QGridLayout()
        self.grid.setSpacing(10)

        self.grid.addWidget(self.lbl1, 0, 0)
        self.grid.addWidget(self.ts_combo1, 1, 0)
        self.grid.addWidget(self.qlist1, 2, 0)
        
        
        self.grid.addWidget(self.lbl2, 0, 1)
        self.grid.addWidget(self.ts_combo2, 1, 1)
        self.grid.addWidget(self.qlist2, 2, 1)
        

        self.grid.addWidget(self.btn, 7, 0)

        self.setLayout(self.grid)

        self.build_qlist1(self.qlist1,2,0)
        self.build_qlist1(self.qlist2,2,1)

        self.setGeometry(300, 300, 450, 200)

        self.show()






    def build_qlist1(self,_obj,r,c):

        if c == 0:
            ta = self.tiles_actual
            n = self.ts_combo1.currentText()
            qtext = self.qlist1_text
            func = self.qlist_clicked1
        elif c == 1:
            ta = self.tiles_actual2
            n = self.ts_combo2.currentText()
            qtext = self.qlist2_text
            func = self.qlist_clicked2

        _obj = QListWidget(self)
        
        _obj.setMinimumWidth(150)
        _obj.setMaximumWidth(200)

        _obj.clear()
        qtext = ["Eraser"]

        def eraser():
            self.eraser_item = QListWidgetItem()
            self.eraser_item.setText("Eraser")
            if os.path.isfile("eraser.png"):
                self.eraser_item.setIcon(QIcon("eraser.png"))

        eraser()
        _obj.addItem(self.eraser_item)     

        self.build_tiles(w.ts_path + n,c+1)
        tiles_actual = ta[n]

        for i in range(len(tiles_actual)):

            item = QListWidgetItem()
            item.setText('Tile ' + str(i))
            item.setIcon(QIcon(QPixmap.fromImage(tiles_actual[i])))
        
            colors = []
            for ii in range(tiles_actual[i].width()):
                for jj in range(tiles_actual[i].height()):
                    colors.append(QColor(tiles_actual[i].pixel(ii,jj)).getRgbF())

            if list(set(colors)) != [(1.0, 0.0, 1.0, 1.0)]:
                _obj.addItem(item)
                qtext += ["Tile " + str(i)]

        _obj.itemClicked.connect(func)

        self.grid.addWidget(_obj, r, c)
        self.setLayout(self.grid)

    def qlist_clicked1(self,curr):
        self.qlist1.selectedItems()
        itext = curr.text()
        if itext == "Eraser":
            ind = -1
        else:
            ind = int(itext.split("Tile ")[-1])
        self.tile_index1 = ind

    def qlist_clicked2(self,curr):
        self.qlist2.selectedItems()
        itext = curr.text()
        if itext == "Eraser":
            ind = -1
        else:
            ind = int(itext.split("Tile ")[-1])
        self.tile_index2 = ind

    def build_tiles(self,path_to_tileset_image,x):

        if x == 1:
            tsn = self.tile_set_name
            ta = self.tiles_actual
        elif x == 2:
            tsn = self.tile_set_name2
            ta = self.tiles_actual2
        # print(x)


        tsn = path_to_tileset_image.split("\\")[-1]
        if tsn in ta.keys():
            del ta[tsn]
        temp_list = []
        temp_list2 = []
        img = QImage(path_to_tileset_image,"PNG")
        for i in range(0,w.editor.tile_size):
            for j in range(0,w.editor.tile_size):
                tile_image = img.copy(w.editor.tile_size*j,w.editor.tile_size*i,w.editor.tile_size,w.editor.tile_size)
                temp_list2.append(tile_image)
        ta[tsn] = temp_list2


    def execute(self):

        fti = self.tile_index1

        # if itext == "Eraser":
        #     ind = -1
        # else:
        #     ind = int(itext.split("Tile ")[-1])
        self.qlist2.setSelection



        if fti == -1:
            ftsn = "-1"
        else:
            ftsn = self.ts_combo1.currentText()

        tti = self.tile_index2
        if tti == -1:
            ttsn = "-1"
        else:
            ttsn = self.ts_combo2.currentText()

        print(fti,ftsn,tti,ttsn)

        for j in range(0,w.editor.board_height):
            for i in range(0,w.editor.board_width):
                ti = w.editor.board[j][i].tile_index
                tsn = w.editor.board[j][i].tile_set_name
                w.editor.board_prev[j][i].tile_index = ti
                w.editor.board_prev[j][i].tile_set_name = tsn

                if ti == fti and tsn == ftsn:
                    w.editor.board[j][i].tile_index = tti
                    w.editor.board[j][i].tile_set_name = ttsn

        w.repaint()



if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MyWidget()
    w.show()
    # w.center()
    w.repaint()

    app.setQuitOnLastWindowClosed(False)


    app.exec_()

