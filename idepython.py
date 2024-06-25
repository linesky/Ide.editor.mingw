import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox
import subprocess
import os

class CodeEditorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Code Editor")
        self.root.configure(bg='yellow')

        self.filename_var = tk.StringVar(value="new.c")

        self.entry_filename = tk.Entry(root, textvariable=self.filename_var, width=30)
        self.entry_filename.pack(padx=10, pady=5)

        self.text_area = scrolledtext.ScrolledText(root, width=80, height=20)
        self.text_area.pack(padx=10, pady=10)

        self.frame_buttons = tk.Frame(root, bg='yellow')
        self.frame_buttons.pack(padx=10, pady=5)

        self.button_new = tk.Button(self.frame_buttons, text="New", command=self.new_file)
        self.button_new.grid(row=0, column=0, padx=5)

        self.button_load = tk.Button(self.frame_buttons, text="Load", command=self.load_file)
        self.button_load.grid(row=0, column=1, padx=5)

        self.button_save = tk.Button(self.frame_buttons, text="Save", command=self.save_file)
        self.button_save.grid(row=0, column=2, padx=5)

        self.button_compile = tk.Button(self.frame_buttons, text="Compile", command=self.compile_file)
        self.button_compile.grid(row=0, column=3, padx=5)

        self.button_run = tk.Button(self.frame_buttons, text="Run", command=self.run_file)
        self.button_run.grid(row=0, column=4, padx=5)

    def new_file(self):
        self.text_area.delete(1.0, tk.END)
        self.filename_var.set("new.c")

    def load_file(self):
        filename = filedialog.askopenfilename(defaultextension=".c",
                                              filetypes=[("C files", "*.c"), ("All files", "*.*")])
        if filename:
            with open(filename, 'r') as file:
                content = file.read()
                self.text_area.delete(1.0, tk.END)
                self.text_area.insert(tk.END, content)
                self.filename_var.set(filename)

    def save_file(self):
        filename = self.filename_var.get()
        with open(filename, 'w') as file:
            content = self.text_area.get(1.0, tk.END)
            file.write(content.strip())
            messagebox.showinfo("Save", f"File '{filename}' saved successfully.")

    def compile_file(self):
        filename = self.filename_var.get()
        exe_name = os.path.splitext(filename)[0] + '.out'
        compile_command = f"gcc {filename} -o {exe_name}"
        process = subprocess.Popen(compile_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()
        if process.returncode == 0:
            messagebox.showinfo("Compile", f"Compilation successful.\nExecutable: {exe_name}")
        else:
            messagebox.showerror("Compile Error", stderr.decode())
        
    def run_file(self):
        filename = self.filename_var.get()
        exe_name = os.path.splitext(filename)[0] + '.out'
        if os.path.exists(exe_name):
            run_command = f'/usr/bin/xterm -bg yellow -fg black -e ./{exe_name}'
            process = os.system(run_command)
            
            

if __name__ == "__main__":
    root = tk.Tk()
    app = CodeEditorApp(root)
    root.mainloop()

