using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.IO.Ports;

namespace ProtonBridgeTestApp
{

    public partial class Form1 : Form
    {
        SerialPort port = new SerialPort();
        float Dose; //Накопленная доза - мЗ\ч
        float DoseRate; //МЭД - мкЗ\ч
        int CPS; //скорость счета - имп. в сек.
        ulong ImpCount = 1000000000; //общее количество импульсов

        int Sens; //чувствительность датчика - имп\мкР

        public Form1()
        {
            InitializeComponent();
        }

        //Загрузка формы
        private void Form1_Load(object sender, EventArgs e)
        {

            //Добавляем найденные порты в список
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                comboBox1.Items.Add(port);
            }

            try { comboBox1.SelectedIndex = 1;  } catch { } //Выбираю второй порт в списке своей системы
        }



        //Обновить список СОМ портов системы
        private void button1_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();

            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                comboBox1.Items.Add (port);
            }

            
        }

        //Открытие порта
        private void button2_Click(object sender, EventArgs e)
        {
            try
            {

                // настройки порта 115200 бод, 8O1 
                port.PortName = comboBox1.SelectedItem.ToString();
                port.BaudRate = 115200;
                port.DataBits = 8;
                port.Parity = System.IO.Ports.Parity.Odd;
                port.StopBits = System.IO.Ports.StopBits.One;
                port.ReadTimeout = 2000;
                port.WriteTimeout = 2000;

                port.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

                port.Open();
                button1.Enabled = false;
                button2.Enabled = false;
                button3.Enabled = true;
                button4.Enabled = true;
                button5.Enabled = false;
                comboBox1.Enabled = false;
            }
            catch (Exception exp)
            {
                MessageBox.Show("ERROR: невозможно открыть порт:" + exp.ToString());
                return;
            }
        }

        //Закрытие порта
        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                port.Close(); //Закрыть порт
                button1.Enabled = true;
                button2.Enabled = true;
                button3.Enabled = false;
                button4.Enabled = false;
                button5.Enabled = true;
                comboBox1.Enabled = true;
            }
            catch (Exception exp)
            {
                MessageBox.Show("ERROR: невозможно закрыть порт:" + exp.ToString());
                return;
            }
        }



        //Отправка данных в порт
        private void button4_Click(object sender, EventArgs e)
        {
            int flags=0;

            if (checkBox1.Checked == true) flags |= 1; else flags &= ~1;  //Принудительно включаем бит номер 1 или выключаем
            if (checkBox2.Checked == true) flags |= 2; else flags &= ~2;
            if (checkBox3.Checked == true) flags |= 4; else flags &= ~4;
            if (checkBox4.Checked == true) flags |= 8; else flags &= ~8;
            if (checkBox5.Checked == true) flags |= 16; else flags &= ~16;
            if (checkBox6.Checked == true) flags |= 32; else flags &= ~32;
            if (checkBox7.Checked == true) flags |= 64; else flags &= ~64;
            if (checkBox8.Checked == true) flags |= 128; else flags &= ~128;

            //Флаги
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x00), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(byte.Parse(flags.ToString())), 0, 1); //Шлем данные переменной флагов

            //Накопленная доза
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x01), 0, 1); //Шлем адрес переменной флагов
            if (checkBox9.Checked) port.Write(BitConverter.GetBytes(Dose),0,4); else port.Write(BitConverter.GetBytes(float.Parse(textBox3.Text.ToString())), 0, 4);

            //Мощность дозы
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x02), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(float.Parse(textBox1.Text.ToString())), 0, 4);

            //Количество импульсов за 2 секунды
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x03), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(short.Parse(textBox4.Text.ToString())*2), 0, 2); //Обязательно CPS умножить на 2, как требует профиль

            //Процент заряда батареи
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x04), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(byte.Parse(textBox5.Text.ToString())), 0, 1);

            //Температура
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x05), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(byte.Parse(textBox6.Text.ToString())), 0, 1);


        }

        //Отображение принятых данных в окно лога
         private void DataReceivedHandler(
                    object sender,
                    SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();


            BeginInvoke(new MethodInvoker(delegate
            {
                textBox2.AppendText(indata);
            }));

        }

        //Событие закрытия формы
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                port.Close(); //Закрыть порт
            }
            catch (Exception exp)
            {

            }
        }

        //Включение таймера симуляции работы дозиметра
        private void checkBox9_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox9.Checked) timer1.Enabled = true; else timer1.Enabled = false; 
        }

        //Событие таймера - тикает каждую секунду
        private void timer1_Tick(object sender, EventArgs e)
        {
            Random rand1 = new Random();                //Формируем рандомные показания CPS
            CPS = rand1.Next(84, 86);                   //Число импульсов в секунду
            ImpCount = ImpCount + (ulong)CPS;           //Суммарное число импульсов
            DoseRate = (CPS) /1700f * 3600f /100;       //МАЭД в мкЗв/ч
            Dose = ImpCount / (100f * 1700f) / 1000f;   //Накопленная доза в мЗв


            textBox3.Text = (Dose).ToString(); //Накопленная доза
            textBox1.Text = DoseRate.ToString(); //МЭД
            textBox4.Text = (CPS).ToString(); //Число импульсов за последние 1 секунды
            textBox7.Text= ImpCount.ToString(); //Общее число импульсов


            if (button2.Enabled == false)
            {
                button4_Click(null, null); //Обновляем только основную измерительную характеристику
            }
        }

        //Кнопка записи данных второй измерительной характеристики
        private void button5_Click(object sender, EventArgs e)
        {
            //Общее число импульсов
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x10), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(ulong.Parse(textBox7.Text.ToString())), 0, 8);

            //Число импульсов коррекции
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x11), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(uint.Parse(textBox10.Text.ToString())), 0, 4);

            //Общее число импульсов за N секунд.
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x12), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(uint.Parse(textBox8.Text.ToString())), 0, 4);

            //Время измерения дозы в секундах..
            port.Write(BitConverter.GetBytes(0xA0), 0, 1); //Шлем команду записи данных
            port.Write(BitConverter.GetBytes(0x13), 0, 1); //Шлем адрес переменной флагов
            port.Write(BitConverter.GetBytes(uint.Parse(textBox9.Text.ToString())), 0, 4);


        }


    }
}
