import subprocess
import tkinter as tk
from functools import partial
import rospy
import os

class LaunchControlApp:
    def __init__(self, root):
        self.root = root
        self.root.title('Launch文件控制')
        self.root.geometry("800x1000")
        
        # 创建四个按钮
        self.button1 = tk.Button(root, text='启动预备', command=partial(self.launch, 'start_pre.launch'),width=200,height=7).pack(anchor='center')
        self.button2 = tk.Button(root, text='红区启动', command=partial(self.launch, 'start_red.launch'),width=200,height=7).pack(anchor='center')
        self.button3 = tk.Button(root, text='红区重启', command=partial(self.launch, 'restart_red.launch'),width=200,height=7).pack(anchor='center')
        self.button4 = tk.Button(root, text='蓝区启动', command=partial(self.launch, 'start_blue.launch'),width=200,height=7).pack(anchor='center')
        self.button5 = tk.Button(root, text='蓝区重启', command=partial(self.launch, 'restart_blue.launch'),width=200,height=7).pack(anchor='center')
        self.button6 = tk.Button(root, text='程序终止', command=partial(self.launch, 'shutdown.launch'),width=200,height=7).pack(anchor='center')

    def launch(self, filename):
        # 启动对应的launch文件

        # 关闭其他按钮对应的launch文件
        if filename == 'start_pre.launch':
            self.close_launch(['start_red.launch', 'restart_red.launch', 'start_blue.launch', 'restart_blue.launch', 'shutdown.launch'])
        elif filename == 'start_red.launch':
            self.close_launch(['start_pre.launch', 'restart_red.launch', 'start_blue.launch', 'restart_blue.launch', 'shutdown.launch'])
        elif filename == 'restart_red.launch':
            self.close_launch(['start_pre.launch', 'start_red.launch', 'start_blue.launch', 'restart_blue.launch', 'shutdown.launch'])
        elif filename == 'start_blue.launch':
            self.close_launch(['start_pre.launch', 'start_red.launch', 'restart_red.launch', 'restart_blue.launch', 'shutdown.launch'])
        elif filename == 'restart_blue.launch':
            self.close_launch(['start_pre.launch', 'start_red.launch', 'restart_red.launch', 'start_blue.launch', 'shutdown.launch'])
        elif filename == 'shutdown.launch':
            self.close_launch(['start_pre.launch', 'start_red.launch', 'restart_red.launch', 'start_blue.launch', 'restart_blue.launch'])
        subprocess.Popen(['roslaunch', 'r2_decision',filename])

    def close_launch(self, filenames):
        for filename in filenames:
            os.system("ekill %s" % filename)


if __name__ == '__main__':
    root = tk.Tk()
    app = LaunchControlApp(root)
    rospy.init_node("test_ui")
    r = rospy.Rate(1)
    while not rospy.is_shutdown():
        root.mainloop()
        r.sleep()
