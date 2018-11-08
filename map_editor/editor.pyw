import pandas as pd
import numpy as np
import os

import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QKeyEvent, QPainter,QImage, QPen, QIcon, QPixmap, QColor, QBrush, QCursor, QFont
from PyQt5.QtCore import Qt, QPoint, QPointF, QSize, QEvent


class Tile:
    def __init__(self):
        self.tile_index = -1
        self.tile_set_name = "-1"

class Object:
    def __init__(self):
        self.png = ""
        self.x = 0
        self.y = 0

class Editor(QWidget):

    def __init__(self, parent=None):
        QWidget.__init__(self, parent)

        self.init_variables()
        self.setMouseTracking(True)

    
    def init_variables(self):
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
        self.rectangle_wh = False
        self.grid = True
        self.c1_ghost = (0,0)
        self.c4_ghost = (0,0)
        self.mouse_x = 0
        self.mouse_y = 0
        self.ts_path = ""

        self.painted_objects = []
        self.objects = {}
        self.object_name = ""
        self.object_erasing = False
        self.draw_objs = True
        self.align_objs = True



    def edit_board(self,x,y):
        if not self.draw_over and self.board[y][x].tile_index != -1 and self.tile_index != -1 and self.tool != "fill":
            return
        self.board[y][x].tile_index = self.tile_index
        if self.tile_index == -1:
            self.board[y][x].tile_set_name = "-1"
        else:
            self.board[y][x].tile_set_name = self.tile_set_name

    def build_objects(self,path,lst):
        self.objects = {}
        for l in lst:
            img = QImage(path + l,"PNG")
            color = QColor(1.0, 0.0, 1.0, 0)
            for y in range(img.height()):
                for x in range(img.width()):
                    col = QColor(img.pixel(x,y)).getRgbF()
                    if col == (1.0, 0.0, 1.0, 1.0):
                        img.setPixelColor(x,y,color)
            self.objects[l] = img
        


    def build_tiles(self,path_to_tileset_image):
        self.tile_set_name = path_to_tileset_image.split("\\")[-1]
        if self.tile_set_name in self.tiles.keys():
            del self.tiles[self.tile_set_name]
            del self.tiles_actual[self.tile_set_name]

        temp_list = []
        temp_list2 = []
        img = QImage(path_to_tileset_image,"PNG")
        # print(path_to_tileset_image)
        for i in range(0,self.tile_size):
            for j in range(0,self.tile_size):
                tile_image = img.copy(self.tile_size*j,self.tile_size*i,self.tile_size,self.tile_size)
                temp_list.append(tile_image.scaledToHeight(self.tile_size_zoom))
                temp_list2.append(tile_image)

        self.tiles[self.tile_set_name] = temp_list
        self.tiles_actual[self.tile_set_name] = temp_list2



    def paintEvent(self, event):

        self.draw_tiles()
        if self.draw_objs:
            self.draw_objects()
        self.draw_grid()
        self.draw_rect_wh(self.h_lbound,self.v_lbound)
        self.draw_coords(self.h_lbound+2,self.v_ubound-5,int(int(self.mouse_x / self.tile_size_zoom) * self.tile_size),int(int(self.mouse_y / self.tile_size_zoom) * self.tile_size))
        self.draw_coords(self.h_ubound-55,self.v_ubound-5,int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))
        self.draw_copy_status(self.h_lbound + (self.h_ubound - self.h_lbound)*.45,self.v_ubound-5)
        self.drawGhostRect()
        

    def draw_tiles(self):
        painter = QPainter(self)
        vl = max(int(self.v_lbound / self.tile_size_zoom),0)
        vu = min(int(self.v_ubound / self.tile_size_zoom)+1,self.board_height)
        hl = max(int(self.h_lbound / self.tile_size_zoom),0)
        hu = min(int(self.h_ubound / self.tile_size_zoom)+1,self.board_width)

        for x in range(hl,hu):
            for y in range(vl,vu):
                if(self.board[y][x].tile_index > -1):
                    s = self.board[y][x]
                    if s.tile_set_name != "":
                        if not s.tile_set_name in self.tiles.keys():
                            if os.path.isfile(self.ts_path + s.tile_set_name):
                                self.build_tiles(self.ts_path + s.tile_set_name)
                            else:
                                continue
                        painter.drawImage(x*self.tile_size_zoom,y*self.tile_size_zoom,self.tiles[s.tile_set_name][s.tile_index])

    def draw_objects(self):
        painter = QPainter(self)
        for i in range(len(self.painted_objects)):
            x = int(self.painted_objects[i].x * self.zoom_ratio)
            y = int(self.painted_objects[i].y * self.zoom_ratio)
            png = self.painted_objects[i].png
            if png in self.objects.keys():
                img = self.objects[png]
                img = img.scaledToHeight(int(img.height() * self.zoom_ratio))
                painter.drawImage(x,y,img)


    def draw_copy_status(self,x,y):
        if self.tool != "copy range":
            return
        
        text = ""
        if not self.copied_range and not self.copying:
            text = "select a range to copy"
        elif self.copied_range and not self.copying:
            text = "click to paste copied range"
        elif self.copying:
            text = ""

        qp = QPainter()
        qp.begin(self)
        qp.drawText(QPointF(x,y),text)
        qp.end()


    def draw_grid(self):
        if self.tile_size_zoom >= 4 and self.grid:
            qp = QPainter()
            qp.begin(self)
            pen = QPen(Qt.gray, 1, Qt.SolidLine)
            qp.setPen(pen)
            for y in range(0,self.board_height):
                qp.drawLine(y*self.tile_size_zoom, 0, y*self.tile_size_zoom, 4096*self.zoom_ratio)
            for x in range(0,self.board_width):
                qp.drawLine(0, x*self.tile_size_zoom, 4096*self.zoom_ratio, x*self.tile_size_zoom)
            qp.end()

    def drawGhostRect(self):
        qp = QPainter()

        if self.tool in ["pen","rectangle","rectangle fill","copy range"]:
            qp.begin(self)

            Xrange = range(min([self.c1_ghost[0],self.c4_ghost[0]]),max([self.c1_ghost[0],self.c4_ghost[0]])+1)
            Yrange = range(min([self.c1_ghost[1],self.c4_ghost[1]]),max([self.c1_ghost[1],self.c4_ghost[1]])+1)

            x = min(Xrange)*self.tile_size_zoom
            y = min(Yrange)*self.tile_size_zoom

            w = (max(Xrange) - min(Xrange)+1)*self.tile_size_zoom
            h = (max(Yrange) - min(Yrange)+1)*self.tile_size_zoom
            # print(x,y,w,h)

            brush = QBrush(QColor(128, 128, 255, 128))
            qp.fillRect(x,y,w,h,brush)
            qp.end()
        
        elif self.tool in ["objects"]:
            qp.begin(self)

            if self.align_objs:
                x = int(self.mouse_x / self.tile_size_zoom) * self.tile_size_zoom
                y = int(self.mouse_y / self.tile_size_zoom) * self.tile_size_zoom
            else:
                x = self.mouse_x
                y = self.mouse_y

            if self.object_name.lower() == "eraser":
                w = self.tile_size_zoom
                h = self.tile_size_zoom
            else:
                img = self.objects[self.object_name]
                w = int(img.width() * self.zoom_ratio)
                h = int(img.height() * self.zoom_ratio)
            brush = QBrush(QColor(128, 128, 255, 128))
            qp.fillRect(x,y,w,h,brush)
            qp.end()

    def draw_rect_wh(self,x,y):
        if self.rectangle_wh:
            qp = QPainter()
            qp.begin(self)
            x1 = self.c1[0]; x2 = self.c4[0]; y1 = self.c1[1]; y2 = self.c4[1]
            w = max([x1,x2]) - min([x1,x2]) + 1
            h = max([y1,y2]) - min([y1,y2]) + 1
            # qp.drawText(QPointF(x+2,y+32),"W: " + str(w) + " H: " + str(h))
            qp.drawText(QPointF(x+2,y+12),"W: " + str(w) + " H: " + str(h))
            qp.end()
            self.rectangle_wh = False

    def draw_coords(self,x,y,dx,dy):
        qp = QPainter()
        qp.begin(self)
        qp.drawText(QPointF(x,y),"(" + str(dx) + "," + str(dy) + ")")
        # qp.drawText(QPointF(x+2,y-5),"(" + str(self.mouse_x) + "," + str(self.mouse_y) + ")")
        # qp.drawText(QPointF(x+2,y-5),"(" + str(int(self.mouse_x / self.tile_size_zoom)) + "," + str(int(self.mouse_y / self.tile_size_zoom)) + ")")
        qp.end()


    def mousePressEvent(self, event):
        self.mouse_x = event.pos().x()
        self.mouse_y = event.pos().y()
        x = int(self.mouse_x / self.tile_size_zoom)
        y = int(self.mouse_y / self.tile_size_zoom)

        if event.button() == Qt.RightButton:
            menu = QMenu()
            ti = self.board[y][x].tile_index
            tsn = self.board[y][x].tile_set_name
            a0 = menu.addAction('Tile Set: ' + tsn)
            a1 = menu.addAction('Tile Index: ' + str(ti))
            action = menu.exec_(event.globalPos())



        if event.button() == Qt.LeftButton:

            for j in range(0,self.board_height):
                for i in range(0,self.board_width):
                    self.board_prev[j][i].tile_index = self.board[j][i].tile_index
                    self.board_prev[j][i].tile_set_name = self.board[j][i].tile_set_name

            if self.tool == "pen":
                self.pen_tool("press")

            elif self.tool == "rectangle":
                self.rectangle_tool("press")

            elif self.tool == "rectangle fill":
                self.rectangle_tool("press")

            elif self.tool == "copy range":
                self.copy_tool("press")

            elif self.tool == "fill":
                self.flood_fill(x,y)
            
            elif self.tool == "objects":
                self.objects_tool("press")
                        
        self.update()

    def mouseMoveEvent(self, event):
        self.mouse_x = event.pos().x()
        self.mouse_y = event.pos().y()

        if self.tool == "pen":
            self.pen_tool("move")

        elif self.tool == "rectangle":
            self.rectangle_tool("move")

        elif self.tool == "rectangle fill":
            self.rectangle_tool("move")

        elif self.tool == "copy range":
            self.copy_tool("move")
        
        elif self.tool == "objects":
            self.objects_tool("move")

        self.update()

    def mouseReleaseEvent(self, event):
        self.mouse_x = event.pos().x()
        self.mouse_y = event.pos().y()

        if self.tool == "pen":
            self.pen_tool("release")

        elif self.tool == "rectangle":
            self.rectangle_tool("release")

        elif self.tool == "rectangle fill":
            self.rectangle_tool("release")
        
        elif self.tool == "copy range":
            self.copy_tool("release")

        elif self.tool == "objects":
            self.objects_tool("release")

        self.update()



    def pen_tool(self,mouse):
        x = int(self.mouse_x / self.tile_size_zoom)
        y = int(self.mouse_y / self.tile_size_zoom)

        side = self.bsize-1
        x1 = x-int(side/2)
        x2 = x+int(side/2)+side % 2
        y1 = y-int(side/2)
        y2 = y+int(side/2)+side % 2

        self.c1_ghost = (x1,y1)
        self.c4_ghost = (x2,y2)
        
        if mouse == "press":
            self.painting = True
            self.draw_area(x1, x2, y1, y2)

        elif mouse == "move":
            if self.painting:
                self.draw_area(x1, x2, y1, y2)

        elif mouse == "release":
            self.painting = False

    def objects_tool(self,mouse):
        x = int(self.mouse_x / self.tile_size_zoom)
        y = int(self.mouse_y / self.tile_size_zoom)
        if self.align_objs:
            ox = x * self.tile_size_zoom / self.zoom_ratio
            oy = y * self.tile_size_zoom / self.zoom_ratio
        else:
            ox = self.mouse_x / self.zoom_ratio
            oy = self.mouse_y / self.zoom_ratio

        def check_eraser(ox,oy):
            elx = ox
            eux = elx + self.tile_size_zoom
            ely = oy
            euy = oy + self.tile_size_zoom
            newlst = []
            for pa in self.painted_objects:
                img = self.objects[pa.png]
                h = int(img.height() * self.zoom_ratio)
                w = int(img.width() * self.zoom_ratio)
                lx = pa.x
                ux = lx + w
                ly = pa.y
                uy = ly + h

                x_collision = False
                y_collision = False
                if (elx <= ux and elx >= lx) or (eux <= ux and eux >= lx) or (eux >= ux and elx <= lx):
                    x_collision = True
                if (ely <= uy and ely >= ly) or (euy <= uy and euy >= ly) or (euy >= uy and elx <= ly):
                    y_collision = True
                if not(x_collision and y_collision):
                    newlst.append(pa)
            self.painted_objects = newlst

        if mouse == "press":

            if not self.object_name.lower() in ["","eraser"]:
                obj = Object()
                obj.x = ox
                obj.y = oy
                obj.png = self.object_name
                self.painted_objects.append(obj)

            elif self.object_name.lower() == "eraser":
                check_eraser(ox,oy)
                self.object_erasing = True

        elif mouse == "move":
            if self.object_name.lower() == "eraser" and self.object_erasing:
                check_eraser(ox,oy)
        
        elif mouse == "release":
            self.object_erasing = False
            

    def rectangle_tool(self,mouse):
        x = int(self.mouse_x / self.tile_size_zoom)
        y = int(self.mouse_y / self.tile_size_zoom)

        if mouse == "press":
            self.c1 = (x,y)
            self.c1_ghost = (x,y)
            self.c4_ghost = (x,y)
            self.rectangling = True


        elif mouse == "move":
            if self.rectangling:
                self.c4 = (x,y)
                self.rect_moved = True
                self.c4_ghost = (self.c4[0],self.c4[1])

                self.rectangle_wh = True

            elif not self.rectangling:
                self.c1_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))
                self.c4_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))

            

        elif mouse == "release":
            if not self.rect_moved:
                self.c4 = (self.c1[0],self.c1[1])
            self.c1_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))
            self.c4_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))
            self.rectangling = False
            
            if self.tool == "rectangle": 
                self.c2 = (self.c1[0],self.c4[1])
                self.c3 = (self.c4[0],self.c1[1])
                # c1 - c2; c1 - c3; c3 - c4; c2 - c4

                x1 = self.c1[0]; x2 = self.c2[0]; y1 = self.c1[1]; y2 = self.c2[1]
                self.draw_area(x1,x2,y1,y2)

                x1 = self.c1[0]; x2 = self.c3[0]; y1 = self.c1[1]; y2 = self.c3[1]
                self.draw_area(x1,x2,y1,y2)

                x1 = self.c3[0]; x2 = self.c4[0]; y1 = self.c3[1]; y2 = self.c4[1]
                self.draw_area(x1,x2,y1,y2)

                x1 = self.c2[0]; x2 = self.c4[0]; y1 = self.c2[1]; y2 = self.c4[1]
                self.draw_area(x1,x2,y1,y2)

            elif self.tool == "rectangle fill":
                x1 = self.c1[0]; x2 = self.c4[0]; y1 = self.c1[1]; y2 = self.c4[1]
                self.draw_area(x1,x2,y1,y2)

            self.rect_moved = False


    def copy_tool(self,mouse):
        x = int(self.mouse_x / self.tile_size_zoom)
        y = int(self.mouse_y / self.tile_size_zoom)
        
        if mouse == "press":
            # starting to copy a range
            if not self.copied_range:
                self.c1_copy = (x,y)
                self.c1_ghost = (x,y)
                self.c4_ghost = (x,y)
                self.copying = True
        
            # pasting range
            elif self.copied_range:
                
                # copied range
                Xcrange = range(min([self.c1_copy[0],self.c4_copy[0]]),max([self.c1_copy[0],self.c4_copy[0]])+1)
                Ycrange = range(min([self.c1_copy[1],self.c4_copy[1]]),max([self.c1_copy[1],self.c4_copy[1]])+1)

                self.c1 = (x,y)
                self.c4 = (x+max(Xcrange)-min(Xcrange),y+max(Ycrange)-min(Ycrange))
                
                # paste range
                Xrange = range(min([self.c1[0],self.c4[0]]),max([self.c1[0],self.c4[0]])+1)
                Yrange = range(min([self.c1[1],self.c4[1]]),max([self.c1[1],self.c4[1]])+1)     

                board_temp = [[Tile() for x in range(self.board_width)] for y in range(self.board_height)]
                for x in range(self.board_width):
                    for y in range(self.board_height):
                        board_temp[y][x].tile_index = self.board[y][x].tile_index
                        board_temp[y][x].tile_set_name = self.board[y][x].tile_set_name   

                for x in Xrange:
                    for y in Yrange:
                        if x >= 0 and x < self.board_width and y >= 0 and y < self.board_height:
                            
                            xc = Xcrange[Xrange.index(x)]
                            yc = Ycrange[Yrange.index(y)]

                            if xc >= 0 and xc < self.board_width and yc >= 0 and y < self.board_height:

                                tile_index = board_temp[yc][xc].tile_index
                                tile_set_name = board_temp[yc][xc].tile_set_name

                                self.board[y][x].tile_index = tile_index
                                if tile_index == -1:
                                    self.board[y][x].tile_set_name = "-1"
                                else:
                                    self.board[y][x].tile_set_name = tile_set_name

                self.copied_range = False
                self.copying = False

        elif mouse == "move":

            # if still copying
            if self.copying:
                self.c4_copy = (x,y)
                self.copy_moved = True
                self.c4_ghost = (self.c4_copy[0],self.c4_copy[1])

            # if you have a copied range 
            elif self.copied_range:
                # copied range
                Xcrange = range(min([self.c1_copy[0],self.c4_copy[0]]),max([self.c1_copy[0],self.c4_copy[0]])+1)
                Ycrange = range(min([self.c1_copy[1],self.c4_copy[1]]),max([self.c1_copy[1],self.c4_copy[1]])+1)
                # paste range
                self.c1_ghost = (x,y)
                self.c4_ghost = (x+max(Xcrange)-min(Xcrange),y+max(Ycrange)-min(Ycrange))

            # if just moving the mouse around
            elif self.tool == "copy range":
                self.c1_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))
                self.c4_ghost = (int(self.mouse_x / self.tile_size_zoom),int(self.mouse_y / self.tile_size_zoom))

        elif mouse == "release":
            # if finishing copying
            if self.copying:
                if not self.copy_moved:
                    self.c4_copy = (self.c1_copy[0],self.c1_copy[1])
                self.c4_ghost = (self.c4_copy[0],self.c4_copy[1])
                self.copying = False
                self.copied_range = True
                self.copy_moved = False

            # if pasting
            elif self.copied_range:
                if not self.copy_moved:
                    self.c4_copy = (self.c1_copy[0],self.c1_copy[1])
                self.c4_ghost = (self.c4_copy[0],self.c4_copy[1])
                self.copy_moved = False



    def draw_area(self,x1,x2,y1,y2):
        Xrange = range(min([x1,x2]),max([x1,x2])+1)
        Yrange = range(min([y1,y2]),max([y1,y2])+1)
        for x in Xrange:
            for y in Yrange:
                if x >= 0 and x < self.board_width and y >= 0 and y < self.board_height:
                    self.edit_board(x,y)


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

        # self.board[y][x].tile_index    = self.tile_index
        # self.board[y][x].tile_set_name = self.tile_set_name
        self.edit_board(x,y)

        node = (y,x)
        queue = []
        queue.append(node)
        
        while(queue != []):
            n = queue[0]
            queue = queue[1:]

            # right
            if (n[0] + 1 < self.board_width):
                if(self.board[n[0]+1][n[1]+0].tile_index == target_tile_index and self.board[n[0]+1][n[1]+0].tile_set_name == target_tile_set_name):
                    self.board[n[0]+1][n[1]+0].tile_index    = self.tile_index
                    self.board[n[0]+1][n[1]+0].tile_set_name = to_tile_set_name
                    queue.append((n[0]+1,n[1]+0))

            # left
            if (n[0] - 1 >= 0):
                if(self.board[n[0]-1][n[1]+0].tile_index == target_tile_index and self.board[n[0]-1][n[1]+0].tile_set_name == target_tile_set_name):
                    self.board[n[0]-1][n[1]+0].tile_index    = self.tile_index
                    self.board[n[0]-1][n[1]+0].tile_set_name = to_tile_set_name
                    queue.append((n[0]-1,n[1]+0))
            
            # down
            if (n[1] + 1 < self.board_height):
                if(self.board[n[0]+0][n[1]+1].tile_index == target_tile_index and self.board[n[0]+0][n[1]+1].tile_set_name == target_tile_set_name):
                    self.board[n[0]+0][n[1]+1].tile_index    = self.tile_index
                    self.board[n[0]+0][n[1]+1].tile_set_name = to_tile_set_name
                    queue.append((n[0]+0,n[1]+1))

            # up
            if (n[1] - 1 >= 0):
                if(self.board[n[0]+0][n[1]-1].tile_index == target_tile_index and self.board[n[0]+0][n[1]-1].tile_set_name == target_tile_set_name):
                    self.board[n[0]+0][n[1]-1].tile_index    = self.tile_index
                    self.board[n[0]+0][n[1]-1].tile_set_name = to_tile_set_name
                    queue.append((n[0]+0,n[1]-1))


if __name__ == '__main__':
    pass

