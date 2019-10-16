from kivy.app import App
from kivy.core.window import Window
from kivy.uix.button import  Button
from kivy.uix.textinput import TextInput
from kivy.uix.gridlayout import GridLayout
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
 
class MySynth(App):

    def loadPreset():
        pass

    def savePreset():
        pass
 
    def build(self):
        numLoadedPresets = 0

        #create presets sidebar
        
        self.presetsSidebar = StackLayout(orientation='lr-tb', size_hint=(.2,1))

        for i in range(1, 10):

            self.preset = Button(text='Preset ' + str(i), size_hint_x=1, size_hint_y=None, height=50)

            self.presetsSidebar.add_widget(self.preset)

            numLoadedPresets+=1

        #self.presetsSidebar.remove_widget(self.presetsSidebar.children[4])

        #tutorial stuff
        self.box2 = BoxLayout(orientation='horizontal', spacing=20)
 
        self.txt = TextInput(hint_text='Write here', size_hint=(.5,.1))
 
        self.btn = Button(text='Clear All', on_press=self.clearText, size_hint=(.1,.1))
 
        self.box2.add_widget(self.txt)
 
        self.box2.add_widget(self.btn)
 
        return self.presetsSidebar
 
    def clearText(self, instance):
 
        self.txt.text = ''

    #save/load dialog functions
    def dismiss_popup(self):
        self._popup.dismiss()

    def show_load(self):
        content = LoadDialog(load=self.load, cancel=self.dismiss_popup)
        self._popup = Popup(title="Load file", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def show_save(self):
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



if __name__ == '__main__':
    MySynth().run()