import multiprocessing
import subprocess
import time
import string
import random

def run_nc(file_name):
    nick = ''.join(random.choices(string.ascii_letters, k=10))
    with open(file_name, 'r') as file:
        lines = file.readlines()
    for i, line in enumerate(lines):
        if line.startswith('NICK'):
            lines[i] = 'NICK {}\n'.format(nick)
    for i, line in enumerate(lines):
        if line.startswith('OPER'):
            lines[i] = 'OPER {} .\n'.format(nick)
    for i, line in enumerate(lines):
        if line.startswith('MODE'):
            time.sleep(20)
    with open(file_name, 'w') as file:
        file.writelines(lines)
    command = "nc 127.0.0.1 1234 < {} &".format(file_name)
    subprocess.run(command, shell=True)


def main():
    file_list = ['input1.txt', 'input2.txt', 'input3.txt', 'input4.txt', 'input5.txt',
                 'input6.txt', 'input7.txt', 'input8.txt', 'input9.txt', 'input10.txt',
                 'input11.txt', 'input12.txt', 'input13.txt', 'input14.txt', 'input15.txt',
                 'input16.txt', 'input17.txt', 'input18.txt', 'input19.txt', 'input20.txt',
                 'input21.txt']
    processes = []
    try:
        for i in range(4000):
            print("Ejecutando tarea {}".format(i))
            p = multiprocessing.Process(target=run_nc,args=(random.choice(file_list),))
            p.start()
            if (i % 2 == 0):
                p.kill()
            processes.append(p)
            time.sleep(0.3)
        for p in processes:
            p.join()
        time.sleep(20)
        for p in processes:
            p.terminate()
    except KeyboardInterrupt:
        print("Interrupción por Control+C detectada, terminando procesos...")
        for p in processes:
            p.terminate()
        print("Todos los procesos han sido terminados.")

# Ejecuta la función main
if __name__ == '__main__':
    multiprocessing.freeze_support()
    main()