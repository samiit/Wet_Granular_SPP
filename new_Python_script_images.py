import os
import re
a=[]
file_list=[]

a=[]
file_list=[]
for file_name in os.listdir("./images/"):
    b = int(float(file_name[5:-4])*20) + 1
    a.append(b)

t_max = str(max(a))
t_max_len = len(t_max)

for file_name in os.listdir("./images/"):
    t_file = int(float(file_name[5:-4])*20) + 1
    t_file_len = len(str(t_file))
    zeros_needed = t_max_len - t_file_len
    file_name_mod = "image_" + "0" * zeros_needed + str(t_file)+".png"
    os.rename("./images/"+file_name, "./images/"+file_name_mod)

