from tkinter import *

nums = [0,0,0,0]

def get_nums(event):
    nums = []
  
    with open('/dev/etx_device', 'w') as f:
        print('show1', file=f)

    with open('/dev/etx_device', 'r') as f:
        content = f.readlines()
        nums.append(content[0])

    with open('/dev/etx_device', 'w') as f:
        print('show2', file=f)

    with open('/dev/etx_device', 'r') as f:
        content = f.readlines()
        nums.append(content[0])

    with open('/dev/etx_device', 'w') as f:
        print('show3', file=f)

    with open('/dev/etx_device', 'r') as f:
        content = f.readlines()
        nums.append(content[0])

    with open('/dev/etx_device', 'w') as f:
        print('show4', file=f)

    with open('/dev/etx_device', 'r') as f:
        content = f.readlines()
        nums.append(content[0])
    print(nums)
    label1.config(text = "Монет достоинством "+ "1" + " - " + str(nums[0]) + " Штук")  
    label2.config(text = "Монет достоинством "+ "2" + " - " + str(nums[1]) + " Штук")
    label3.config(text = "Монет достоинством "+ "5" + " - " + str(nums[2]) + " Штук")
    label4.config(text = "Монет достоинством "+ "10" + " - " + str(nums[3]) + " Штук")


def clear_ca(event):
    with open('/dev/etx_device', 'w') as f:
        print('clear', file=f)


def setup_ca(event):

    def setp(event):
        n = e1.get()
        v = e2.get()
        p = e3.get()
        with open('/dev/etx_device', 'w') as f:
            print('set ',n, ' ', v, ' ', p, file=f)

    t = Toplevel()
    t.title("Настройка")

    label1 = Label(t, text = "Номер")
    label1.pack()    

    e1 = Entry(t, width=50)
    e1.pack()
    e1.insert(0, '1')


    label2 = Label(t, text = "Номинал")
    label2.pack()    

    e2 = Entry(t, width=50)
    e2.pack()
    e2.insert(0, '1')

    label3 = Label(t, text = "Количество импульсов")
    label3.pack()    

    e3 = Entry(t, width=50)
    e3.pack()
    e3.insert(0, '1')

    button1=Button(t, text='Настроить',width=25,height=5,font='arial 14')
    button1.pack()
    button1.bind('<Button-1>', setp)





root=Tk()
root.title("Coin acceptor driver")

label1 = Label(text = "Монет достоинством "+ "1" + " - " + str(nums[0]) + " Штук")
label1.pack()

label2 = Label(text = "Монет достоинством "+ "2" + " - " + str(nums[1]) + " Штук")
label2.pack()

label3 = Label(text = "Монет достоинством "+ "5" + " - " + str(nums[2]) + " Штук")
label3.pack()

label4 = Label(text = "Монет достоинством "+ "10" + " - " + str(nums[3]) + " Штук")
label4.pack()


button1=Button(root,text='Обновить',width=25,height=5,font='arial 14')
button1.pack()
button1.bind('<Button-1>', get_nums)

button2=Button(root,text='Очистить',width=25,height=5,font='arial 14')
button2.pack()
button2.bind('<Button-1>', clear_ca)

button3=Button(root,text='Настроить',width=25,height=5,font='arial 14')
button3.pack()
button3.bind('<Button-1>', setup_ca)

root.mainloop()
