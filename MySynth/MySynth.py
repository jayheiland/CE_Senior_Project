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

import serial, time, json, os, threading

ser = serial.Serial() #serial object for communcating with the synth

read_timer = None #a timer thread for periodically reading the synth's values

presets_dict = {}

module_1_name = "VCO - Pitch"
module_2_name = "Low Pass Filter - Cutoff"
module_3_name = "High Pass Filter - Cutoff"

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
        self.name_button = Button(size_hint=(.8,1), on_press=self.update_modules)
        self.add_widget(self.name_button)
        self.close_button = Button(text='X', size_hint=(.2,1), on_press=self.remove_pip)
        self.add_widget(self.close_button)
        self.filepath = ''
    
    def update_modules(self, instance):
        self.parent.parent.children[0].update_modules_from_preset(self.name_button.text)
    
    def remove_pip(self, instance):
        self.parent.remove_preset(self.name_button.text)

class PresetsSidebar(StackLayout):
    def __init__(self, **kwargs):
        super(PresetsSidebar, self).__init__(**kwargs)
        self.orientation = 'lr-tb'
        self.size_hint = (.2,1)
    
    def remove_preset(self, name):
        for child in self.children:
            if child.name_button.text == name:
                self.remove_widget(child)
                del presets_dict[name]
                print("Closed preset. Loaded presets are now: " + str(presets_dict.keys()))
                return

class Module(BoxLayout):
    def __init__(self, **kwargs):
        super(Module, self).__init__(**kwargs)
        self.orientation = 'vertical'
        self.size_hint = (None, None)
        self.size = (180, 100)
        self.label = Label(text='Module', size_hint=(1,.6))
        self.add_widget(self.label)
        self.input = TextInput(text='0', size_hint=(1,.4), multiline=False, on_text_validate=self.on_enter_module_value, on_double_tap=self.on_select_module_input)
        self.add_widget(self.input)

    def on_select_module_input(self, instance):
        global read_timer
        if(read_timer != None):
            read_timer.cancel()
        self.parent.restart_read_timer() #cancel the reading timer while entering values, otherwise the app will continue to overwrite the GUI values
    
    def on_enter_module_value(self, instance):
        #due to the initial serial setup, the communcation with the STM should be hanging after the "Resistor?" prompt
        global ser
        res_index = 0
        #find the index of the corresponding resistor
        module_name = self.label.text
        hex_string = str(hex(int(self.input.text)))
        self.parent.write_synth(hex_string, module_name)

class ModulesPanel(StackLayout):
    def __init__(self, **kwargs):
        super(ModulesPanel, self).__init__(**kwargs)
        self.orientation = 'lr-tb'
        self.size_hint = (.8,1)
        self.mod_1 = Module()
        self.mod_1.label.text = module_1_name
        self.add_widget(self.mod_1)
        self.mod_2 = Module()
        self.mod_2.label.text = module_2_name
        self.add_widget(self.mod_2)
        self.mod_3 = Module()
        self.mod_3.label.text = module_3_name
        self.add_widget(self.mod_3)
    
    def read_synth(self):
        #the STM only writes the encoder values to the digipots when this program writes "r"
        print("Executing read_synth")
        global ser
        if(ser.name != None):
            ser.write(b'r')
            reads = []
            while True:
                line = ser.readline()
                reads.append(str(line))
                if b'Resistor?\n' in line:
                    break
            print(reads)
            module_vals = []
            for i in range(0,3):
                module_vals.append(reads[i][len(reads[i])-5:len(reads[i])-3])
            print(module_vals)
            #write digipot values to GUI modules (convert: hex -> int -> string)
            for m in range(0,3):
                hex_str = "0x" + str(module_vals[m])
                self.children[2-m].input.text = str(int(hex_str,base=16))
                print(self.children[2-m].label.text + ": " + self.children[2-m].input.text)
        self.restart_read_timer()
    
    def write_synth(self, hex_string, module_name):
        #due to the initial serial setup, the communcation with the STM should be hanging after the "Resistor?" prompt
        global ser
        global read_timer
        if(read_timer != None):
            read_timer.cancel()
        
        res_index = 0
        #find the index of the corresponding resistor
        if(module_name == module_1_name):
            res_index = 1
        elif(module_name == module_2_name):
            #res_index = 5
            return
        elif(module_name == module_3_name):
            #res_index = 6
            return
        
        wrote_values = False
        if(ser.name != None):
            print("Writing to synth")
            ser.write(b'r')
            if(len(hex_string) == 3):
                hex_string = hex_string[:2] + '0' + hex_string[2:]
            print(hex_string)
            while True:
                line = ser.readline()
                print(line)
                if b'Resistor?\n' in line:
                    if(wrote_values):
                        break
                    ser.write(str(res_index).encode("utf-8"))
                elif b'Value MSN?\n' in line:
                    ser.write(hex_string[2].upper().encode("utf-8"))
                elif b'Value LSN?\n' in line:
                    ser.write(hex_string[3].upper().encode("utf-8"))
                    wrote_values = True
            print("Set '" + module_name + "' to " + str(int(hex_string,base=16)) + " on a 0-255 scale")
        self.restart_read_timer()
    
    def restart_read_timer(self):
        #reset "read synth" timer thread
        global ser
        global read_timer
        if(ser.name != None):
            read_timer = threading.Timer(5.0, self.read_synth)
            read_timer.start()
    
    def update_modules_from_preset(self, preset):
        global presets_dict
        self.update_modules_values(presets_dict[preset])

    def update_modules_values(self, data):
        global read_timer
        if(read_timer != None):
            read_timer.cancel()
        for key, value in data.items():
            for module in self.children:
                if key == module.label.text:
                    module.input.text = str(value)
                    hex_string = str(hex(int(module.input.text)))
                    self.write_synth(hex_string, module.label.text)
                    time.sleep(0.25)
        print("Updated GUI values to: " + str(data))
        self.restart_read_timer()


class MySynth(App):
    
    def build(self):
        self.root = BoxLayout(orientation='horizontal', size_hint_min_x=500)
        #create presets sidebar
        self.presets_sidebar = PresetsSidebar()

        self.savePresetButton = Button(text='Save', size_hint_x=.5, size_hint_y=None, height=30, size_hint_min_x=50, on_release=self.show_save)
        self.presets_sidebar.add_widget(self.savePresetButton)

        self.loadPresetButton = Button(text='Load', size_hint_x=.5, size_hint_y=None, height=30, size_hint_min_x=50, on_release=self.show_load)
        self.presets_sidebar.add_widget(self.loadPresetButton)

        self.connectButton = Button(text='Connect', size_hint_x=.5, size_hint_y=None, height=30, size_hint_min_x=50, on_release=self.show_connect)
        self.presets_sidebar.add_widget(self.connectButton)
        self.root.add_widget(self.presets_sidebar)

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
    
    def show_connect(self, instance):
        content = LoadDialog(load=self.connect, cancel=self.dismiss_popup)
        self._popup = Popup(title="Connect device", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def show_save(self, instance):
        content = SaveDialog(save=self.save, cancel=self.dismiss_popup)
        self._popup = Popup(title="Save file", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def load(self, path, filename):
        global presets_dict
        print("path is " + str(path))
        print("filename is " + str(filename))
        with open(os.path.join(path, filename[0])) as infile:
            data = json.load(infile)
            if not self.add_preset_button(path, filename, data):
                self.dismiss_popup()
                return
            print(str(data))
        self.dismiss_popup()
    
    #setup the serial port to communicate with the STM
    def connect(self, path, filename):
        global ser
        global read_timer
        filename = str(filename).replace("['", "")
        filename = filename.replace("']", "")
        filename = filename.replace("cu.", "tty.")
        ser = serial.Serial(filename, 115200, timeout=1)
        print("Device path is: " + str(filename))
        print("Baud rate: " + str(ser.baudrate))
        print("Port name: " + ser.name)
        print(ser.write(b'r'))
        while True:
            line = ser.readline()
            print(line)
            if (b'Resistor?\n' in line):
                break
        if(read_timer != None):
            read_timer.cancel()
        self.modules.restart_read_timer()
        self.dismiss_popup()

    def save(self, path, filename):
        if ".preset" not in filename:
            filename = filename + ".preset"
        with open(os.path.join(path, filename), 'w') as outfile:
            json.dump(self.get_modules_data(), outfile)
        self.dismiss_popup()

    def get_modules_data(self):
        data = {}
        for module in self.modules.children:
            data[module.label.text] = int(module.input.text)
        return data
    
    def add_preset_button(self, path, filename, data):
        preset_loaded = False
        for preset in self.presets_sidebar.children:
            if preset is PresetPip:
                if str(filename) == preset.filepath:
                    preset_loaded = True
        if preset_loaded == False:
            self.preset = PresetPip()
            filename_split = str(filename).split("/")
            filename_split = filename_split[len(filename_split)-1].split(".")
            for key in presets_dict.keys():
                if(key == filename_split[0]):
                    return False
            self.preset.name_button.text = filename_split[0]
            self.preset.filepath = str(filename)
            presets_dict[self.preset.name_button.text] = data
            self.presets_sidebar.add_widget(self.preset)
            self.modules.update_modules_values(data)
            return True
    
    def on_stop(self):
        if(read_timer != None):
            read_timer.cancel()
        time.sleep(1)
        if(ser != None):
            ser.close()
        print("Closing serial port")




if __name__ == '__main__':
    Window.clearcolor = (.3,.3,.4,1)
    MySynth().run()