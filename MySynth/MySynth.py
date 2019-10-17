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

import os

class LoadDialog(FloatLayout):
    load = ObjectProperty(None)
    cancel = ObjectProperty(None)


class SaveDialog(FloatLayout):
    save = ObjectProperty(None)
    text_input = ObjectProperty(None)
    cancel = ObjectProperty(None)

class Module(BoxLayout):
    def __init__(self, **kwargs):
        super(Module, self).__init__(**kwargs)
        self.orientation = 'vertical'
        self.size_hint = (None, None)
        self.size = (200, 200)
        self.label = Label(text='VCO 1', size_hint=(1,.7))
        self.add_widget(self.label)
        self.input = TextInput(hint_text='Value', size_hint=(1,.3), multiline=False, on_text_validate=self.on_enter)
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

        for i in range(1, 10):
            self.preset = Button(text='Preset ' + str(i), size_hint_x=1, size_hint_y=None, height=50, size_hint_min_x=100)
            self.presetsSidebar.add_widget(self.preset)

        #self.presetsSidebar.remove_widget(self.presetsSidebar.children[4])
        self.root.add_widget(self.presetsSidebar)

        #create modules panel
        self.root.add_widget(ModulesPanel())


        #tutorial stuff
        self.box2 = BoxLayout(orientation='horizontal', spacing=20)
 
        self.txt = TextInput(hint_text='Write here', size_hint=(.5,.1))
 
        self.btn = Button(text='Clear All', on_press=self.clearText, size_hint=(.1,.1))
 
        self.box2.add_widget(self.txt)
 
        self.box2.add_widget(self.btn)
 
        return self.root
 
    def clearText(self, instance):
 
        self.txt.text = ''

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
        #with open(os.path.join(path, filename[0])) as stream:
        #    widgets to write to go here = stream.read()

        self.dismiss_popup()

    def save(self, path, filename):
        #with open(os.path.join(path, filename), 'w') as stream:
        #    stream.write('stuff to write to save file goes here')

        self.dismiss_popup()

#register classes with the Factory for use throughout project
#Factory.register('MySynth', cls=MySynth)
#Factory.register('LoadDialog', cls=LoadDialog)
#Factory.register('SaveDialog', cls=SaveDialog)
#Factory.register('Module', cls=Module)
#Factory.register('ModulesPanel', cls=ModulesPanel)




if __name__ == '__main__':
    MySynth().run()