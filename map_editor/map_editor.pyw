import pandas as pd
import numpy as np
import os

# from shutil import which
# from getpass import getuser
# from time import strftime, sleep, time, mktime
# from datetime import datetime
# from subprocess import Popen

import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QKeyEvent, QPainter,QImage, QPen, QIcon, QPixmap, QColor, QBrush, QCursor, QFont
from PyQt5.QtCore import Qt, QPoint, QPointF, QSize, QEvent


class Tile:
    def __init__(self):
        self.tile_index = -1
        self.tile_set_name = "-1"

class Editor(QWidget):

    def __init__(self, parent=None):
        QWidget.__init__(self, parent)

        self.tiles = {}
        self.tiles_actual = {}
        self.tile_set_name = ""
        self.tile_index = 0
        self.board_width  = 256
        self.board_height = 256
        self.tile_size = 16
        self.zoom_ratio = 16/16
        self.tile_size_zoom = int(self.tile_size * self.zoom_ratio)
        self.v_lbound = 0
        self.v_ubound = self.board_height * self.tile_size_zoom
        self.h_lbound = 0
        self.h_ubound = self.board_width * self.tile_size_zoom
        self.board = [[Tile() for i in range(self.board_width)] for j in range(self.board_height)]
        self.board_prev = [[Tile() for i in range(self.board_width)] for j in range(self.board_height)]
        self.painting = False
        self.tool = "pen"
        self.bsize = 1
        self.copied_range = False
        self.copy_moved = False
        self.copying = False
        self.draw_over = True
        self.rect_moved = False
        self.rectangling = False
        self.c1_ghost = (0,0)
        self.c4_ghost = (0,0)
        self.mouse_x = 0
        self.mouse_y = 0
        self.setMouseTracking(True)
        self.draw_grid()
    
    def edit_board(self,x,y):
        if not self.draw_over and self.board[y][x].tile_index != -1 and self.tile_index != -1 and self.tool != "fill":
            return
        self.board[y][x].tile_index = self.tile_index
        if self.tile_index == -1:
            self.board[y][x].tile_set_name = "-1"
        else:
            self.board[y][x].tile_set_name = self.tile_set_name

    def build_tiles(self,path_to_tileset_image):
        self.tile_set_name = path_to_tileset_image.split("\\")[-1]
        if self.tile_set_name in self.tiles.keys():
            del self.tiles[self.tile_set_name]
            del self.tiles_actual[self.tile_set_name]
        
        temp_list = []
        temp_list2 = []
        img = QImage(path_to_tileset_image,"PNG")
        
        for i in range(0,self.tile_size):
            for j in range(0,self.tile_size):
                temp_list.append(img.copy(self.tile_size*j,self.tile_size*i,self.tile_size_zoom,self.tile_size_zoom))
                temp_list2.append(img.copy(self.tile_size*j,self.tile_size*i,self.tile_size,self.tile_size))
        self.tiles[self.tile_set_name] = temp_list
        self.tiles_actual[self.tile_set_name] = temp_list2
    
    

    def flood_fill(self,x,y):

        target_tile_index    = self.board[y][x].tile_index
        target_tile_set_name = self.board[y][x].tile_set_name
        if target_tile_index == -1:
            target_tile_set_name = "-1"
        if self.tile_index == -1:
            to_tile_set_name = "-1"
        else:
            to_tile_set_name = self.tile_set_name

        if(target_tile_index == self.tile_index and target_tile_set_name == to_tile_set_name):
            return

        # print("target ind:",target_tile_index)
        # print("to ind:",self.tile_index)
        # print("target set:",target_tile_set_name)
        # print("to set:",to_tile_set_name)
        # print("left ind:",self.board[y+1][x].tile_index)
        # print("left set:",self.board[y+1][x].tile_set_name)

        # self.board[y][x].tile_index    = self.tile_index
        # self.board[y][x].tile_set_name = self.tile_set_name
        self.edit_board(x,y)

        node = (y,x)
        queue = []
        queue.append(node)
        
        while(queue != []):
            n = queue[0]
            queue = queue[1:]
            # print(len(queue))

            # right
            if (n[0] + 1 < self.board_width):
                if(self.board[n[0]+1][n[1]+0].tile_index == target_tile_index and self.board[n[0]+1][n[1]+0].tile_set_name == target_tile_set_name):
                    self.board[n[0]+1][n[1]+0].tile_index    = self.tile_index
                    self.board[n[0]+1][n[1]+0].tile_set_name = to_tile_set_name
                    # self.edit_board(n[0]+1,n[1]+0)
                    queue.append((n[0]+1,n[1]+0))

            # left
            if (n[0] - 1 >= 0):
                if(self.board[n[0]-1][n[1]+0].tile_index == target_tile_index and self.board[n[0]-1][n[1]+0].tile_set_name == target_tile_set_name):
                    self.board[n[0]-1][n[1]+0].tile_index    = self.tile_index
                    self.board[n[0]-1][n[1]+0].tile_set_name = to_tile_set_name
                    # self.edit_board(n[0]-1,n[1]+0)
                    queue.append((n[0]-1,n[1]+0))
            
            # down
            if (n[1] + 1 < self.board_height):
                if(self.board[n[0]+0][n[1]+1].tile_index == target_tile_index and self.board[n[0]+0][n[1]+1].tile_set_name == target_tile_set_name):
                    self.board[n[0]+0][n[1]+1].tile_index    = self.tile_index
                    self.board[n[0]+0][n[1]+1].tile_set_name = to_tile_set_name
                    # self.edit_board(n[0]+0,n[1]+1)
                    queue.append((n[0]+0,n[1]+1))

            # up
            if (n[1] - 1 >= 0):
                if(self.board[n[0]+0][n[1]-1].tile_index == target_tile_index and self.board[n[0]+0][n[1]-1].tile_set_name == target_tile_set_name):
                    self.board[n[0]+0][n[1]-1].tile_index    = self.tile_index
                    self.board[n[0]+0][n[1]-1].tile_set_name = to_tile_set_name
                    # self.edit_board(n[0]+0,n[1]-1)
                    queue.append((n[0]+0,n[1]-1))

    def paintEvent(self, event):

        painter = QPainter(self)

        vl = max(int(self.v_lbound / self.tile_size_zoom),0)
        vu = min(int(self.v_ubound / self.tile_size_zoom)+1,self.board_height)
        hl = max(int(self.h_lbound / self.tile_size_zoom),0)
        hu = min(int(self.h_ubound / self.tile_size_zoom)+1,self.board_width)

        # print("vertical",vl,vu)
        # print("horizontal",hl,hu)
        # print("----------------------")

        # color all
        # for i in range(0,self.board_width):
        #     for j in range(0,self.board_height):
        #         if(self.board[i][j].tile_index > -1):
        #             s = self.board[i][j]
        #             painter.drawImage(i*self.tile_size_zoom,j*self.tile_size_zoom,self.tiles[s.tile_set_name][s.tile_index])

        # i = horizontal/x
        for i in range(hl,hu):
            # j = vertical/y
            for j in range(vl,vu):
                if(self.board[j][i].tile_index > -1):
                    s = self.board[j][i]
                    painter.drawImage(i*self.tile_size_zoom,j*self.tile_size_zoom,self.tiles[s.tile_set_name][s.tile_index])
                    
        self.draw_grid()
        self.drawGhostRect()

    def drawGhostRect(self):
        qp = QPainter()
        
        if(self.tool == "pen"):
            qp.begin(self)
            side = self.tile_size*self.bsize

            x = max(0.0,self.mouse_x - side/2.0)
            y = max(0.0,self.mouse_y - side/2.0)

            # align ghost with grid
            x = x + (self.tile_size/2) - self.mouse_x % (self.tile_size)
            y = y + (self.tile_size/2) - self.mouse_y % (self.tile_size)

            w = int(side) + side % 2
            h = int(side) + side % 2

            brush = QBrush(QColor(128, 128, 255, 128))
            qp.fillRect(x,y,w,h,brush)
            qp.end()

        if(self.tool in ["rectangle","rectangle fill"]):
            qp.begin(self)
            side = self.tile_size

            # print(self.c1_ghost,self.c4_ghost)

            Xrange = range(min([self.c1_ghost[0],self.c4_ghost[0]]),max([self.c1_ghost[0],self.c4_ghost[0]])+1)
            Yrange = range(min([self.c1_ghost[1],self.c4_ghost[1]]),max([self.c1_ghost[1],self.c4_ghost[1]])+1)

            x = min(Xrange)*self.tile_size
            y = min(Yrange)*self.tile_size

            w = (max(Xrange) - min(Xrange)+1)*self.tile_size
            h = (max(Yrange) - min(Yrange)+1)*self.tile_size
            print(x,y,w,h)

            brush = QBrush(QColor(128, 128, 255, 128))
            qp.fillRect(x,y,w,h,brush)
            qp.end()



    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            for j in range(0,self.board_height):
                for i in range(0,self.board_width):
                    self.board_prev[j][i].tile_index = self.board[j][i].tile_index
                    self.board_prev[j][i].tile_set_name = self.board[j][i].tile_set_name


        if(event.button() == Qt.LeftButton and self.tool == "pen"):
            self.painting = True
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)

            side = self.bsize-1
            self.draw_line(x-int(side/2),
                           x+int(side/2)+side % 2,
                           y-int(side/2),
                           y+int(side/2)+side % 2)
            # self.draw_line(x,x+self.bsize-1,y,y+self.bsize-1)

    

        elif(event.button() == Qt.LeftButton and self.tool == "fill"):
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)
            self.flood_fill(x,y)
        
        elif(event.button() == Qt.LeftButton and self.tool == "rectangle"):
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)

            self.c1 = (x,y)
            self.c1_ghost = (x,y)
            self.c4_ghost = (x,y)
            self.edit_board(x,y)
            self.rectangling = True

        elif(event.button() == Qt.LeftButton and self.tool == "rectangle fill"):
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)

            self.c1 = (x,y)
            self.c1_ghost = (x,y)
            self.c4_ghost = (x,y)
            self.edit_board(x,y)
            self.rectangling = True



        # copy range
        elif(event.button() == Qt.LeftButton and self.tool == "copy range" and not self.copied_range):
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)
            self.c1_copy = (x,y)
            self.copied_range = True
            self.copying = True
        
        # paste range
        elif(event.button() == Qt.LeftButton and self.tool == "copy range" and self.copied_range):
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)
            
            # copied range
            Xcrange = range(min([self.c1_copy[0],self.c4_copy[0]]),max([self.c1_copy[0],self.c4_copy[0]])+1)
            Ycrange = range(min([self.c1_copy[1],self.c4_copy[1]]),max([self.c1_copy[1],self.c4_copy[1]])+1)

            self.c1 = (x,y)
            # # self.c4 = (x+(self.c4_copy[0] - self.c1_copy[0]),y+(self.c4_copy[1] - self.c1_copy[1]))
            self.c4 = (x+max(Xcrange)-min(Xcrange),y+max(Ycrange)-min(Ycrange))
            
            # paste range
            Xrange = range(min([self.c1[0],self.c4[0]]),max([self.c1[0],self.c4[0]])+1)
            Yrange = range(min([self.c1[1],self.c4[1]]),max([self.c1[1],self.c4[1]])+1)

            # board_temp = [[Tile() for x in range(self.board_width)] for y in range(self.board_height)]
            # for x in Xrange:
            #     for y in Yrange:
            #         board_temp[y][x].tile_index = self.board[y][x].tile_index
            #         board_temp[y][x].tile_set_name = self.board[y][x].tile_set_name                 


            board_temp = [[Tile() for x in range(self.board_width)] for y in range(self.board_height)]
            for x in range(self.board_width):
                for y in range(self.board_height):
                    board_temp[y][x].tile_index = self.board[y][x].tile_index
                    board_temp[y][x].tile_set_name = self.board[y][x].tile_set_name   


            for x in Xrange:
                for y in Yrange:
                    if x >= 0 and x < self.board_width and y >= 0 and y < self.board_height:
                        
                        # if not self.draw_over and self.board[y][x].tile_index != -1 and self.tile_index != -1:
                        #     continue

                        xc = Xcrange[Xrange.index(x)]
                        yc = Ycrange[Yrange.index(y)]
                        # tile_index = self.board[yc][xc].tile_index
                        # tile_set_name = self.board[yc][xc].tile_set_name
                        tile_index = board_temp[yc][xc].tile_index
                        tile_set_name = board_temp[yc][xc].tile_set_name

                        self.board[y][x].tile_index = tile_index
                        if tile_index == -1:
                            self.board[y][x].tile_set_name = "-1"
                        else:
                            self.board[y][x].tile_set_name = tile_set_name
            self.copied_range = False


        self.update()

    def mouseReleaseEvent(self, event):
        self.painting = False
        self.copying = False
        self.rectangling = False
        

        if self.tool == "rectangle":

            # reset c4
            if not self.rect_moved:
                self.c4 = (self.c1[0],self.c1[1])
            self.c4_ghost = (self.c4[0],self.c4[1])
            
            self.c2 = (self.c1[0],self.c4[1])
            self.c3 = (self.c4[0],self.c1[1])

            # c1 - c2
            # c1 - c3
            # c3 - c4
            # c2 - c4
            x1 = self.c1[0]
            x2 = self.c2[0]
            y1 = self.c1[1]
            y2 = self.c2[1]
            self.draw_line(x1,x2,y1,y2)

            x1 = self.c1[0]
            x2 = self.c3[0]
            y1 = self.c1[1]
            y2 = self.c3[1]
            self.draw_line(x1,x2,y1,y2)

            x1 = self.c3[0]
            x2 = self.c4[0]
            y1 = self.c3[1]
            y2 = self.c4[1]
            self.draw_line(x1,x2,y1,y2)

            x1 = self.c2[0]
            x2 = self.c4[0]
            y1 = self.c2[1]
            y2 = self.c4[1]
            self.draw_line(x1,x2,y1,y2)

            self.rect_moved = False
            self.c1_ghost = (self.mouse_x,self.mouse_y)
            self.c4_ghost = (self.mouse_x,self.mouse_y)

        elif self.tool == "rectangle fill":

            # reset c4
            if not self.rect_moved:
                self.c4 = (self.c1[0],self.c1[1])
            self.c4_ghost = (self.c4[0],self.c4[1])
   
            x1 = self.c1[0]
            x2 = self.c4[0]
            y1 = self.c1[1]
            y2 = self.c4[1]
            self.draw_line(x1,x2,y1,y2)

            self.rect_moved = False
            self.c1_ghost = (self.mouse_x,self.mouse_y)
            self.c4_ghost = (self.mouse_x,self.mouse_y)
        
        elif self.copied_range and self.tool == "copy range":
            if not self.copy_moved:
                self.c4_copy = (self.c1_copy[0],self.c1_copy[1])
            self.copy_moved = False

        self.update()

    def mouseMoveEvent(self, event):

        self.mouse_x = event.pos().x()
        self.mouse_y = event.pos().y()

        if self.tool == "pen" and self.painting:
            x = int(self.mouse_x / self.tile_size_zoom)
            y = int(self.mouse_y / self.tile_size_zoom)

            side = self.bsize-1
            self.draw_line(x-int(side/2),
                           x+int(side/2)+side % 2,
                           y-int(side/2),
                           y+int(side/2)+side % 2)
            # self.draw_line(x,x+self.bsize-1,y,y+self.bsize-1)

        elif self.tool in ["rectangle","rectangle fill"] and self.rectangling:
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)
            self.c4 = (x,y)
            self.rect_moved = True
            self.c4_ghost = (self.c4[0],self.c4[1])

        elif self.tool in ["rectangle","rectangle fill"] and not self.rectangling:
            self.c1_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))
            self.c4_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))

        elif(self.copied_range and self.tool == "copy range" and self.copying):
            x = int(event.pos().x() / self.tile_size_zoom)
            y = int(event.pos().y() / self.tile_size_zoom)
            self.c4_copy = (x,y)
            self.copy_moved = True



        self.update()

    def draw_line(self,x1,x2,y1,y2):
        Xrange = range(min([x1,x2]),max([x1,x2])+1)
        Yrange = range(min([y1,y2]),max([y1,y2])+1)
        for x in Xrange:
            for y in Yrange:
                if x >= 0 and x < self.board_width and y >= 0 and y < self.board_height:
                    self.edit_board(x,y)

    def draw_grid(self):
        if self.tile_size_zoom >= 4:
            qp = QPainter()
            qp.begin(self)
            pen = QPen(Qt.gray, 1, Qt.SolidLine)
            qp.setPen(pen)
            # rows
            for i in range(0,self.board_height):
                qp.drawLine(i*self.tile_size_zoom, 0, i*self.tile_size_zoom, 4096*self.zoom_ratio)
            # columns
            for j in range(0,self.board_width):
                qp.drawLine(0, j*self.tile_size_zoom, 4096*self.zoom_ratio, j*self.tile_size_zoom)
            qp.end()
        # else:
        #     qp = QPainter()
        #     qp.begin(self)
        #     pen = QPen(Qt.gray, 1, Qt.SolidLine)
        #     qp.setPen(pen)
        #     # rows
        #     for i in range(self.board_height,self.board_height+1):
        #         qp.drawLine(i*self.tile_size_zoom, 0, i*self.tile_size_zoom, 4096*self.zoom_ratio)
        #     # columns
        #     for j in range(self.board_width,self.board_width+1):
        #         qp.drawLine(0, j*self.tile_size_zoom, 4096*self.zoom_ratio, j*self.tile_size_zoom)
        #     qp.end()

class MyWidget(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)

        self.setFont(QFont('Arial', 9))
        QToolTip.setFont(QFont('Arial', 9))


        self.root = os.path.dirname(os.path.abspath(__file__))  + "\\"
        self.ts_path = r"X:\DEPO\SOFTWARE\INHOUSE\GAMES\adventure\data\tilesets" + "\\"
        if not os.path.isdir(self.ts_path):
            self.ts_path = r"C:\Users\Kameron\Desktop\map_editor\tilesets" + "\\"
        self.tilesets = [x for x in os.listdir(self.ts_path) if ".tileset.png" in x]

        self.housekeeping()

        self.editor = Editor(self)
        self.editor.setMinimumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMaximumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMinimumHeight(self.editor.board_height*self.editor.tile_size_zoom)
        self.editor.setMaximumHeight(self.editor.board_height*self.editor.tile_size_zoom)

        

        self.scroll = self.scroll_area(self.editor)
        self.scroll.verticalScrollBar().valueChanged.connect(self.repaint)
        self.scroll.horizontalScrollBar().valueChanged.connect(self.repaint)
        

        self.ts_combo = QComboBox(self)
        for i in self.tilesets:
            self.ts_combo.addItem(i)

        if "terrain.tileset.png" in self.tilesets:
            self.editor.build_tiles(self.ts_path + "terrain.tileset.png")
            index = self.ts_combo.findText("terrain.tileset.png", Qt.MatchFixedString)
            self.ts_combo.setCurrentIndex(index)

        self.ts_combo.activated[str].connect(self.load_tileset)
        self.ts_combo.setToolTip("Ctrl + T")

        self.tool_combo = QComboBox(self)
        self.tool_list = ['Pen','Rectangle','Rectangle Fill','Fill','Copy Range']
        self.tool_combo.addItems(self.tool_list)
        self.tool_combo.activated[str].connect(self.change_tool)
        self.tool_combo.setToolTip("Ctrl + D")

        # self.size_combo = QComboBox(self)
        # self.size_combo.addItems(['1','2','4'])
        # self.size_combo.activated[str].connect(self.change_size)

        self.sld = QSlider(Qt.Horizontal, self)
        self.sld.setMinimum(1)
        self.sld.setMaximum(50)
        self.sld.setMaximumWidth(200)
        self.sld.valueChanged.connect(self.change_size)

        self.sld_zoom = QSlider(Qt.Horizontal, self)
        self.sld_zoom.setMinimum(1)
        self.zoom_values = [1,2,4,8,16]
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

        self.load_btn = QPushButton('Load', self)
        self.load_btn.clicked.connect(self.load_map)
        self.load_btn.resize(self.load_btn.sizeHint())
        self.load_btn.setToolTip("Ctrl + O")

        self.clear_btn = QPushButton('Clear', self)
        self.clear_btn.clicked.connect(self.clear_map)
        self.clear_btn.resize(self.clear_btn.sizeHint())

        self.undo_btn = QPushButton('Undo', self)
        self.undo_btn.clicked.connect(self.undo)
        self.undo_btn.resize(self.undo_btn.sizeHint())
        self.undo_btn.setToolTip("Ctrl + Z")

        self.draw_over_chk = QCheckBox("Draw Over Tiles",self)
        self.draw_over_chk.setChecked(True)
        self.draw_over_chk.installEventFilter(self)

        self.lbl_pen_size = QLabel('Pen Size: 1', self)

        self.lbl_zoom = QLabel('Zoom: x16', self)


        self.list_tiles()

        self.grid = QGridLayout()
        self.grid.setSpacing(10)
        self.grid.addWidget(self.scroll, 0, 0, 10, 1)
        self.grid.addWidget(self.tool_combo, 0, 1)
        self.grid.addWidget(self.lbl_pen_size, 1, 1)
        self.grid.addWidget(self.sld, 2, 1)
        self.grid.addWidget(self.draw_over_chk, 3, 1)
        self.grid.addWidget(self.ts_combo, 4, 1)
        self.grid.addWidget(self.qlist, 5, 1, 5, 1) # also change in load_tileset()

        self.grid.addWidget(self.save_btn, 0, 2)
        self.grid.addWidget(self.load_btn, 1, 2)
        self.grid.addWidget(self.clear_btn, 3, 2)
        self.grid.addWidget(self.undo_btn, 4, 2)
        self.grid.addWidget(self.save2_btn, 5, 2)
        self.grid.addWidget(self.lbl_zoom, 6, 2)
        self.grid.addWidget(self.sld_zoom, 7, 2,1,1)
        # self.grid.addWidget(QLabel("",self), 8, 2)
        # self.grid.setAlignment(Qt.AlignTop)
        
        self.setLayout(self.grid)

        self.setGeometry(300, 300, 1000, 500)
        
        self.center()

        self.installEventFilter(self)

    def change_zoom(self):
        value = self.sld_zoom.value()
        zoom = 1/self.zoom_values[value-1]
        # zoom = 1/(min([abs(x-value) for x in values]) + value)
        # print(zoom)

        self.editor.zoom_ratio = zoom
        self.editor.tile_size_zoom = int(self.editor.tile_size * zoom)
        self.editor.setMinimumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMaximumWidth(self.editor.board_width*self.editor.tile_size_zoom)
        self.editor.setMinimumHeight(self.editor.board_height*self.editor.tile_size_zoom)
        self.editor.setMaximumHeight(self.editor.board_height*self.editor.tile_size_zoom)

        self.scroll = self.scroll_area(self.editor)
        self.scroll.verticalScrollBar().valueChanged.connect(self.repaint)
        self.scroll.horizontalScrollBar().valueChanged.connect(self.repaint)

        self.grid.addWidget(self.scroll, 0, 0, 10, 1)
        self.setLayout(self.grid)

        self.editor.build_tiles(self.ts_path + self.editor.tile_set_name)

        self.lbl_zoom.setText("Zoom: x" + str(int(self.editor.tile_size*zoom)))

        # self.repaint()
        self.editor.update()
        
    
    def housekeeping(self):
        from getpass import getuser
        self.user = getuser()

        self.appd = "C:\\Users\\"+ self.user +"\\AppData\\Local\\map_editor\\"
        if not(os.path.isdir(self.appd)):
            os.makedirs(self.appd)

        self.board_path = self.ts_path + ""
        self.path_path = self.appd + "PATH"
        if not os.path.isfile(self.path_path):
            with open(self.path_path,'w') as f:
                f.write(self.board_path)
        
        with open(self.path_path,'r') as f:
            self.board_path = f.read().strip()
        
        if not os.path.isdir(self.board_path):
            self.board_path = self.ts_path + ""
            if not os.path.isdir(self.board_path):
                self.board_path = self.root + ""

    def resizeEvent(self, event):
        self.repaint()

    def eventFilter(self,source,event):
        if source is self.draw_over_chk:
            self.editor.draw_over = self.draw_over_chk.isChecked()
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
        # options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getSaveFileName(self,"Save",self.ts_path,"Board Files (*.board);;All Files (*)", options=options)
        if not fileName:
            return 
        
        fileName.replace("/","\\")

        self.board_path = "\\".join(fileName.split("\\")[:-1])
        with open(self.path_path,'w') as f:
            f.write(self.board_path)

        map_string = ""
        tile_set_number = 0
        tile_sets = {}

        for i in self.editor.board:

            lst = []
            for j in i:
                ti = j.tile_index
                tsn = j.tile_set_name

                if not tsn in tile_sets.keys():
                    if tsn == "-1":
                        value = "-1"
                    else:
                        value = str(tile_set_number+0)
                        tile_set_number += 1

                    tile_sets[tsn] = value
                    

                lst.append(tile_sets[tsn] + ":" + str(ti))
            

            map_string += ",".join(lst) + "\n"

            path = self.ts_path
            if path[-1] == "\\":
                path = path[:-1]

            other = ":/board_info" + "\n"
            other += "width_in_tiles=" + str(self.editor.board_width) + "\n"
            other += "height_in_tiles=" + str(self.editor.board_height) + "\n"
            other += "\n"

            other += ":/tileset_info" + "\n"
            other += "tileset_dir=" + path + "\n"
            for k in tile_sets.keys():
                if k != "-1":
                    other += tile_sets[k] + "=" + k + "\n"
            other += "\n"
            

            other += ":/data" + "\n"

        with open(fileName,'w') as f:
            f.write(other)
            f.write(map_string)

    def load_map(self):

        options = QFileDialog.Options()
        # options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getOpenFileName(self,"Save",self.board_path,"Board Files (*.board);;All Files (*)", options=options)
        if not fileName:
            return 

        fileName = fileName.replace("/","\\")

        self.board_path = "\\".join(fileName.split("\\")[:-1])
        with open(self.path_path,'w') as f:
            f.write(self.board_path)
        
        with open(fileName,'r') as f:
            lines = f.read().split("\n")

        # [x for x in lines if ":/" in x]
        lines = [x.strip().lower() for x in lines if x.strip() != ""]
        # inds = [lines.index(x) for x in lines if ":/" in x]
        dlines = lines[lines.index(":/data")+1:]

        w = [x.split("width_in_tiles=")[-1] for x in lines if "width_in_tiles=" in x][0]
        h = [x.split("height_in_tiles=")[-1] for x in lines if "height_in_tiles=" in x][0]
        
        tdir = [x.split("tileset_dir=")[-1] for x in lines if "tileset_dir=" in x][0]
        
        
        ti = lines.index(":/tileset_info")
        # ind = ti+0
        tile_sets = {"-1":"-1"}
        for ind in range(ti+1,len(lines)+1):
            l = lines[ind]
            if ":/" in l:
                break
            if "tileset_dir=" in l:
                continue
            v = l.split("=")[0]
            k = l.split("=")[1]
            if not k in tile_sets.keys():
                tile_sets[k] = v
            self.editor.build_tiles(self.ts_path + k)
        # print(tile_sets)

        self.editor.board_width  = int(w)
        self.editor.board_height = int(h)
        self.editor.board = [[Tile() for i in range(self.editor.board_width)] for j in range(self.editor.board_height)]
            
        for j in range(0,self.editor.board_height):
            jdlines = dlines[j].split(",")
            for i in range(0,self.editor.board_width):
                t = jdlines[i]

                ts = t.split(":")[0] # number
                tsn = [x for x in tile_sets.keys() if tile_sets[x] == ts][0] # tile set name
                tile_index = int(t.split(":")[1])

                if tile_index == -1:
                    tsn = "-1"
                self.editor.board[j][i].tile_index = tile_index
                self.editor.board[j][i].tile_set_name = tsn

                self.repaint()




    def change_tool(self,text):
        self.editor.tool = text.lower()

    # def change_size(self,text):
    #     self.editor.bsize = int(text)

    def change_size(self):
        self.editor.bsize = max(1,int(self.sld.value()))
        self.lbl_pen_size.setText("Pen Size: " + str(self.editor.bsize))
        # self.sld.setValue(self.editor.bsize)


    def repaint(self):
        
        self.v_lbound = self.scroll.verticalScrollBar().value()
        self.v_ubound = self.v_lbound + self.scroll.viewport().height()

        self.h_lbound = self.scroll.horizontalScrollBar().value()
        self.h_ubound = self.h_lbound + self.scroll.viewport().width()

        self.editor.v_lbound = self.v_lbound
        self.editor.v_ubound = self.v_ubound
        self.editor.h_lbound = self.h_lbound
        self.editor.h_ubound = self.h_ubound

        # print("vertical:",self.v_lbound,self.v_ubound)
        # print("horizontal:",self.h_lbound,self.h_ubound)


        
    def load_tileset(self,text):

        self.editor.build_tiles(self.ts_path + text)
        self.editor.tile_index = 0

        # self.qlist.setParent(None)

        self.list_tiles()
        self.grid.addWidget(self.qlist, 5, 1, 5, 1)
        self.setLayout(self.grid)


    def eraser(self):
        self.eraser_item = QListWidgetItem()
        self.eraser_item.setText("Eraser")
        if os.path.isfile("eraser.png"):
            self.eraser_item.setIcon(QIcon("eraser.png"))


    def list_tiles(self):

        self.qlist = QListWidget(self)
        
        self.qlist.setMinimumWidth(150)
        self.qlist.setMaximumWidth(200)

        self.qlist.clear()
        self.qlist_text = ["Eraser"]

        tiles = self.editor.tiles[self.editor.tile_set_name]
        tiles_actual = self.editor.tiles_actual[self.editor.tile_set_name]

        self.eraser()
        self.qlist.addItem(self.eraser_item)     

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
        
        self.qlist.itemClicked.connect(self.qlist_clicked)
    
    def qlist_clicked(self,curr):
    
        self.qlist.selectedItems()
        itext = curr.text()

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
        self.editor.update()



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

if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MyWidget()
    w.show()
    # w.center()
    w.repaint()
    app.exec_()

