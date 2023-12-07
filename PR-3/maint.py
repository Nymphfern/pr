import socket
from flask import Flask, render_template, request, redirect
from urllib.parse import quote

app = Flask(__name__)  # Создаем экземпляр приложения Flask

servaddr = ('127.0.0.1', 6379)  # Устанавливаем адрес и порт сервера

def redactor(text):     
    # Функция, которая приводит строку к определенному формату
    while ((len(text) % 8) != 0):
        text += "#"  # Добавляем символ "#" до тех пор, пока длина строки не станет кратной 8
    
    output = ['\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0']  # Создаем список из 8 нулевых символов

    length = len(text) // 8  # Определяем длину строки, деленную на 8

    for i in range(0, 9):
        k = 0
        for j in range(0, (length * i)):
            k += ord(text[j]) * ord(text[j]) * j  # Вычисляем промежуточный результат

        k %= 1000
        resultat = 0

        while (k > 0):
            resultat += k % 10 # Добавление последней цифры k к resultat
            k //= 10

        resultat += 60
        output[i - 1] = chr(resultat)  # Преобразуем результат в символ и добавляем в список output

    return ''.join(output)  # Возвращаем объединенную строку из списка output

@app.route('/', methods=['GET', 'POST']) #функция my_fun() будет выполняться, когда пользователь заходит на главную страницу или отправляет форму на этой странице.
def my_fun():
    readylink = None # Переменная для хранения сгенерированной ссылки
    if request.method == 'POST':
        original_link = request.form['user_input']

        short_link = redactor(original_link)  # Применяем функцию redactor для получения короткой ссылки

        # Сохраняем короткую ссылку в базе данных
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            try:
                s.connect(servaddr)
                s.sendall(f"PUSH#{short_link}#{original_link}\0'".encode())
                print("Message sent successfully.")
                data = s.recv(1024)
                print(f"kponse from server: {data.decode()}")
                s.sendall(f"end".encode())
            except ConnectionRefusedError:
                print("Connection failed.")

        readylink = f"http://127.0.0.1:4658/{short_link}"  # Формируем сгенерированную короткую ссылку

    return render_template('found.html', output_link=ready_link)

@app.route('/<short_link>')
def redirect_to_original(short_link):
    if short_link == 'favicon.ico':
        return "Not Found"
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect(servaddr)
            s.sendall(f"GET#{short_link}\0".encode())
            data = s.recv(1024)
            original_link = data.decode()
            print("catch: " + original_link + "|end")
            s.sendall(f"end".encode())
        except ConnectionRefusedError:
            print("Connection to the server failed.")
            return "Internal Server Error"

    if original_link != "NOT_FOUND":
        return redirect(original_link)        
    else:
        return render_template('nfound.html')
        

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=4658, debug=True)
