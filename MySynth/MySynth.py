from kivy.app import App
from kivy.uix.label import Label
from kivy.core.window import Window
from kivy.uix.button import  Button
from kivy.uix.textinput import TextInput
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.stacklayout import StackLayout
from kivy.factory import Factory
from kivy.properties import ObjectProperty
from kivy.uix.popup import Popup

import json
import os

class LoadDialog(FloatLayout):
    load = ObjectProperty(None)
    cancel = ObjectProperty(None)

class SaveDialog(FloatLayout):
    save = ObjectProperty(None)
    text_input = ObjectProperty(None)
    cancel = ObjectProperty(None)

class PresetPip(BoxLayout):
    def __init__(self, **kwargs):
        super(PresetPip, self).__init__(**kwargs)
        self.orientation = 'horizontal'
        self.size_hint = (1, None)
        self.size_hint_min_x = 100
        self.height = 50
        self.nameButton = Button(size_hint=(.8,1))
        self.add_widget(self.nameButton)
        self.closeButton = Button(text='X', size_hint=(.2,1))
        self.add_widget(self.closeButton)
        self.filePath = ''

class Module(BoxLayout):
    def __init__(self, **kwargs):
        super(Module, self).__init__(**kwargs)
        self.orientation = 'vertical'
        self.size_hint = (None, None)
        self.size = (180, 100)
        self.label = Label(text='Module', size_hint=(1,.6))
        self.add_widget(self.label)
        self.input = TextInput(text='0', size_hint=(1,.4), multiline=False, on_text_validate=self.on_enter)
        self.add_widget(self.input)

    def on_enter(self, instance):
        print("Set '" + self.label.text + "' to " + self.input.text)

class ModulesPanel(StackLayout):
    def __init__(self, **kwargs):
        super(ModulesPanel, self).__init__(**kwargs)
        self.orientation = 'lr-tb'
        self.size_hint = (.8,1)
        self.VCO_1 = Module()
        self.VCO_1.label.text = "VCO 1"
        self.add_widget(self.VCO_1)
        self.VCO_2 = Module()
        self.VCO_2.label.text = "VCO 2"
        self.add_widget(self.VCO_2)
        self.Filter_1 = Module()
        self.Filter_1.label.text = "Filter 1"
        self.add_widget(self.Filter_1)

class MySynth(App):

    def build(self):
        self.root = BoxLayout(orientation='horizontal', size_hint_min_x=500)

        #create presets sidebar
        
        self.presetsSidebar = StackLayout(orientation='lr-tb', size_hint=(.2,1))

        self.savePresetButton = Button(text='Save', size_hint_x=.5, size_hint_y=None, height=30, size_hint_min_x=50, on_release=self.show_save)
        self.presetsSidebar.add_widget(self.savePresetButton)

        self.loadPresetButton = Button(text='Load', size_hint_x=.5, size_hint_y=None, height=30, size_hint_min_x=50, on_release=self.show_load)
        self.presetsSidebar.add_widget(self.loadPresetButton)

        #self.presetsSidebar.remove_widget(self.presetsSidebar.children[4])
        self.root.add_widget(self.presetsSidebar)

        #create modules panel
        self.modules = ModulesPanel()
        self.root.add_widget(self.modules)

    #save/load dialog functions
    def dismiss_popup(self):
        self._popup.dismiss()

    def show_load(self, instance):
        content = LoadDialog(load=self.load, cancel=self.dismiss_popup)
        self._popup = Popup(title="Load file", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def show_save(self, instance):
        content = SaveDialog(save=self.save, cancel=self.dismiss_popup)
        self._popup = Popup(title="Save file", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def load(self, path, filename):
        print("path is " + str(path))
        print("filename is " + str(filename))
        self.addPresetButton(path, filename)
        with open(os.path.join(path, filename[0])) as infile:
            data = json.load(infile)
            for key, value in data.items():
                for module in self.modules.children:
                    if key == module.label.text:
                        module.input.text = str(value)
        self.dismiss_popup()

    def save(self, path, filename):
        if ".preset" not in filename:
            filename = filename + ".preset"
        with open(os.path.join(path, filename), 'w') as outfile:
            json.dump(self.getModulesData(), outfile)

        self.dismiss_popup()

    def getModulesData(self):
        data = {}
        for module in self.modules.children:
            data[module.label.text] = int(module.input.text)
        return data
    
    def addPresetButton(self, path, filename):
        presetLoaded = False
        for preset in self.presetsSidebar.children:
            if preset is PresetPip:
                if str(filename) == preset.filePath:
                    presetLoaded = True
        if presetLoaded == False:
            self.preset = PresetPip()
            filenameSplit = str(filename).split("/")
            filenameSplit = filenameSplit[len(filenameSplit)-1].split(".")
            self.preset.nameButton.text = filenameSplit[0]
            self.preset.filePath = str(filename)
            self.presetsSidebar.add_widget(self.preset)




if __name__ == '__main__':
    Window.clearcolor = (.3,.3,.4,1)
    MySynth().run()