import pandas as pd
import numpy as np
import os
import ctypes

import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QKeyEvent, QPainter,QImage, QPen, QIcon, QPixmap, QColor, QBrush, QCursor, QFont
from PyQt5.QtCore import Qt, QPoint, QPointF, QSize, QEvent, QTimer



class MyWidget(QMainWindow):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)


        self.font = QFont('Arial', 9)
        self.setFont(self.font)
        QToolTip.setFont(self.font)

        # self.setAcceptDrops(True)

        self.setWindowTitle('Animator')

        self.ico = r"X:\DEPO\SOFTWARE\SUPPORT\excel.ico"
        if os.path.isfile(self.ico):
            self.setWindowIcon(QIcon(self.ico))

        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID("adventure_animator")



        self.root = os.path.dirname(os.path.abspath(__file__))  + "\\"
        self.tile_size = 16
        self.tiles_actual = {}
        
        self.housekeeping()
        self.tilesets = [x for x in os.listdir(self.ts_path) if ".tileset.png" in x]
        self.tile_index = 3
        self.tile_set_name = ""
        self.cycle = [0,1,0,2]
        self.cycle_index = 0
        self.scale = 3
        self.pad = 3
        self.spacing = 3


        self.widget = QWidget()

        self.anim_window = QWidget()
        d = self.tile_size * (self.scale*2 + self.pad*2 + self.spacing)
        self.anim_window.setMinimumWidth(d)
        self.anim_window.setMinimumHeight(d)
        # self.anim_window.setMinimumWidth(400)
        # self.anim_window.setMinimumHeight(300)

        self.sld = QSlider(Qt.Horizontal, self)
        self.sld.setMinimum(1)
        self.sld.setMaximum(7)
        self.sld.setMaximumWidth(200)
        self.sld.valueChanged.connect(self.change_speed)


        # self.lbl_pen_size = QLabel('Pen Size: 1', self)

        self.lbl = QLabel('Speed', self)


        self.line_edit = QLineEdit(self.ad_path,self)
        self.line_edit.setToolTip("Adventure Path")

        self.save_path_btn = QPushButton('Save', self)
        self.save_path_btn.clicked.connect(self.save_path)
        self.save_path_btn.resize(self.save_path_btn.sizeHint())
        self.save_path_btn.setToolTip("Save Adventure path")



        self.build_tile_qlist()
        self.build_ts_combo()
        # self.load_tileset(self.tile_set_name)
        
        self.grid = QGridLayout()
        self.grid.setSpacing(10)

        self.grid.addWidget(self.anim_window, 0, 0, 11, 1)

        self.grid.addWidget(self.lbl, 0, 2)
        self.grid.addWidget(self.sld, 1, 2)
        self.grid.addWidget(self.ts_combo, 2, 2)
        self.grid.addWidget(self.qlist, 3, 2, 8, 1) # also change in load_tileset()

        self.grid.addWidget(self.line_edit,11,0,1,2)
        self.grid.addWidget(self.save_path_btn,11,2)


        # self.grid.addWidget(self.save_btn, 0, 2)
        # self.grid.addWidget(self.load_btn, 1, 2)
        # self.grid.addWidget(self.clear_btn, 3, 2)
        # self.grid.addWidget(self.undo_btn, 4, 2)
        # self.grid.addWidget(self.save2_btn, 5, 2)
        # self.grid.addWidget(self.draw_grid_chk, 6, 2)
        # self.grid.addWidget(self.draw_objs_chk, 7, 2)
        # self.grid.addWidget(self.align_grid_chk, 8, 2)
        # self.grid.addWidget(self.lbl_zoom, 9, 2)
        # self.grid.addWidget(self.sld_zoom, 10, 2)

        self.widget.setLayout(self.grid)
        self.setCentralWidget(self.widget)

        # self.sld_zoom.setValue(self.zoom_values.index(1)+1)

        
        

        self.setGeometry(300, 300, 500, 300)

        
        
        self.center()

        self.load_tileset(self.tile_set_name)

        # self.installEventFilter(self)

        self.save_timer = QTimer()
        self.save_timer.timeout.connect(self.repaint)
        self.save_timer.start(300)

        self.sld.setValue(2)

    def build_tile_qlist(self):

        self.qlist = QListWidget(self)
        
        self.qlist.setMinimumWidth(150)
        self.qlist.setMaximumWidth(200)

        self.qlist.clear()
        self.qlist_text = []

        # self.eraser()
        # self.qlist.addItem(self.eraser_item)     
        if self.tile_set_name in self.tiles_actual.keys():
            tiles_actual = self.tiles_actual[self.tile_set_name]
        else:
            tiles_actual = []

        for i in range(len(tiles_actual)):
            
            if i in [3+x for x in [16*y for y in range(16)]]:
                item = QListWidgetItem()
                item.setText('Tile ' + str(i))
                item.setIcon(QIcon(QPixmap.fromImage(tiles_actual[i])))
            
                colors = []
                for ii in range(tiles_actual[i].width()):
                    for jj in range(tiles_actual[i].height()):
                        colors.append(QColor(tiles_actual[i].pixel(ii,jj)).getRgbF())

                # print(str(i),list(set(colors)))
                if list(set(colors)) != [(1.0, 0.0, 1.0, 0)] and list(set(colors)) != [(0.00392156862745098, 0.0, 0.00392156862745098, 1.0)]:

                    self.qlist.addItem(item)
                    self.qlist_text += ["Tile " + str(i)]

        self.qlist.itemClicked.connect(self.qlist_clicked)
        
    
    def qlist_clicked(self,curr):
        self.qlist.selectedItems()
        itext = curr.text()
        ind = int(itext.split("Tile ")[-1])
        self.tile_index = ind


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
        # self.editor.ts_path = self.ts_path

        self.tilesets = [x for x in os.listdir(self.ts_path) if ".tileset.png" in x]

        # # self.editor.tiles = {}
        self.tiles_actual = {}
        self.tile_set_name = ""
        self.build_tile_qlist()
        self.build_ts_combo()
        self.load_tileset(self.tile_set_name)
        
        self.grid.addWidget(self.ts_combo, 2, 2)
        self.widget.setLayout(self.grid)
        self.setCentralWidget(self.widget)
        # # self.load_tileset("")

    def change_speed(self):
        
        speed = 400/self.sld.value()

        self.save_timer.stop()
        self.save_timer.start(speed)
        

    def load_tileset(self,text):

        if os.path.isfile(self.ts_path + self.ts_combo.currentText()):
            self.build_tiles(self.ts_path + self.ts_combo.currentText())
        self.tile_index = 3

        self.build_tile_qlist()

        self.grid.addWidget(self.qlist, 3, 2, 8, 1)
        self.widget.setLayout(self.grid)
        self.setCentralWidget(self.widget)

    def build_tiles(self,path):

        self.tile_set_name = path.split("\\")[-1]
        if self.tile_set_name in self.tiles_actual.keys():
            del self.tiles_actual[self.tile_set_name]

        temp_list = []
        img = QImage(path,"PNG")



        # print(path_to_tileset_image)
        # color = QColor(1.0, 0.0, 1.0, 0)
        for i in range(0,self.tile_size):
            for j in range(0,self.tile_size):
                tile_image = img.copy(self.tile_size*j,self.tile_size*i,self.tile_size,self.tile_size)
                tile_image = tile_image.scaledToHeight(self.tile_size*self.scale)

                color = QColor(1.0, 0.0, 1.0, 0)
                for y in range(tile_image.height()):
                    for x in range(tile_image.width()):
                        col = QColor(tile_image.pixel(x,y)).getRgbF()
                        if col == (1.0, 0.0, 1.0, 1.0):
                            tile_image.setPixelColor(x,y,color)


                # temp_list.append(tile_image.scaledToHeight(self.tile_size_zoom))
                temp_list.append(tile_image)

        # self.tiles[self.tile_set_name] = temp_list
        self.tiles_actual[self.tile_set_name] = temp_list
        

    def build_ts_combo(self):
        self.ts_combo = QComboBox(self)
        for i in self.tilesets:
            self.ts_combo.addItem(i)

        t = "characters.tileset.png"
        if t in self.tilesets:
            self.build_tiles(self.ts_path + t)
            index = self.ts_combo.findText(t, Qt.MatchFixedString)
            self.ts_combo.setCurrentIndex(index)
            self.tile_set_name = t
        elif len(self.tilesets) > 0:
            self.build_tiles(self.ts_path + self.tilesets[0])
            index = self.ts_combo.findText(self.tilesets[0], Qt.MatchFixedString)
            self.ts_combo.setCurrentIndex(index)
            self.tile_set_name = self.tilesets[0]

        self.ts_combo.activated[str].connect(self.load_tileset)

    def housekeeping(self):
        from getpass import getuser
        self.user = getuser()

        self.appd = "C:\\Users\\"+ self.user +"\\AppData\\Local\\adventure_animator\\"
        if not(os.path.isdir(self.appd)):
            os.makedirs(self.appd)

        self.ad_path = self.root + ""

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
        # self.repaint()
        # qr = self.frameGeometry()
        # cp = QDesktopWidget().availableGeometry()
        # print(qr)
        # print(cp)
        pass

    
    def paintEvent(self, event):
        self.draw_tiles()
        self.cycle_index += 1
        
    def draw_tiles(self):
        painter = QPainter(self)
        xpad = self.tile_size * self.pad
        ypad = self.tile_size * self.pad
        if self.tile_set_name in self.tiles_actual.keys():
            painter.drawImage(xpad + 0,ypad + 0,self.tiles_actual[self.tile_set_name][self.tile_index + self.cycle[self.cycle_index % len(self.cycle)]])

            painter.drawImage(xpad + self.tile_size * (self.scale + self.spacing),ypad + 0,self.tiles_actual[self.tile_set_name][self.tile_index-3 + self.cycle[self.cycle_index % len(self.cycle)]])

            painter.drawImage(xpad + 0,ypad + self.tile_size * (self.scale + self.spacing),self.tiles_actual[self.tile_set_name][self.tile_index+3 + self.cycle[self.cycle_index % len(self.cycle)]])

            painter.drawImage(xpad + self.tile_size * (self.scale + self.spacing),ypad + self.tile_size * (self.scale + self.spacing),self.tiles_actual[self.tile_set_name][self.tile_index+6 + self.cycle[self.cycle_index % len(self.cycle)]])

        
            

        
    def center(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MyWidget()
    w.show()
    w.repaint()
    app.setQuitOnLastWindowClosed(False)
    app.exec_()

