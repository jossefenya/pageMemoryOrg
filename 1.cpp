/*Проверим, как вы поняли paging. Для этого вам предлагается выступить в качестве процессора и преобразовать несколько логических адресов в физические. Формат входных данных следующий:

в первой строке вам даны 3 числа m,q,r≥0 , где q - это количество запросов, на которые вам нужно ответить, r - физический адрес корневой таблицы страниц
следующих m строках записаны пары paddr и value - описание физической памяти, каждая пара значит, что по физическому адресу paddr хранится 64 битное значение value, при этом гарантируется, что все paddr различны, выровнены на границу 8 байт и помещаются в 64 бита
в последних q строках идут целые числа - логические адреса, которые вам нужно преобразовать в физические, для каждого из этих чисел нужно вывести на отдельной строке либо физический адрес, либо слово "fault", если преобразовать логический адрес в физический нельзя.
Считайте, что таблица страниц имеет формат 64 битного режима x86 (4 уровня, каждая страница 4 KB, каждая запись 8 байт, формат записи был показан в лекциях), но вы можете игнорировать все поля, кроме бита присутствия (на картинке бит P - нулевой бит) и собственно физического адреса.

Для всех физических адресов, не указанных во входных данных (среди m пар paddr value), считайте, что по этим адресам хранятся нули.

ВАЖНО: это было неочевидно из видео, но все физические адреса, которые хранятся в записях таблицы страниц должны быть выровнены,
 как минимум, на границу 4096 байт (4Kb), т. е. младшие 12 бит физических адресов всегда равны 0,
 соответственно, хранить младшие биты нет смысла и в записе таблицы страниц они не хранятся - их место занимают специальные флаги. Убедитесь, что вы понимаете приведенный пример.*/
#include <iostream>
#include <clocale>
#include <string>
#include <math.h>
#include <fstream>
#include <ctype.h>
using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");
	ifstream input("input.txt");
	uint64_t m, q, r, phys_addr = 0, cnt_fault = 0;
	uint64_t addr[64], mpl4[9];
	uint64_t mpl4_ = 0, physic = 0, value = 0;
	uint64_t directory_ptr_ = 0;
	uint64_t value_ = 0;
	uint64_t table_ = 0;
	uint64_t offset_ = 0;
	uint64_t directory_ = 0;
	uint64_t size = 0;
	uint64_t cnt_of_phys = 0;
	ofstream fout("output.txt");
	cout << "Write your m, q, r: ";
	input >> m >> q >> r;
	uint64_t paddr_array[m][2];
	cout << "Write your paddr and value: " << endl;
	for (uint64_t i = 0; i < m; i++){
		for (uint64_t j = 0; j < 2; j++){
			input >> paddr_array[i][j];
		}
	}
	//Заполняем массив логических адресов
	uint64_t logic_addr[q];
	cout << "Write your logical address:" << endl;
	for (uint64_t i = 0; i < q; i++){
		input >> logic_addr[i];
	}
	cout << endl;
	for (uint64_t k = 0; k < q; k++){
		mpl4_ = 0;
		physic = 0;
		value = 0;
		directory_ptr_ = 0;
		value_ = 0;
		table_ = 0;
		offset_ = 0;
		directory_ = 0;
		size = 0;
		cnt_of_phys = 0;
		//Переводим логический в двоичную систему
		uint64_t logic = logic_addr[k];
		for (int j = 0; logic != 0; j++){
			addr[j] = logic % 2;
			logic /= 2;
			size++;
		}
		for (int i = size; i < 64; i++){
			addr[i] = 0;
		}
		//Выделяем 9 бит соответсвующие mpl4
		for (int i = 39, j = 0; i <= 47; i++, j++){
			mpl4[j] = addr[i];
		}
		for (int i = 0; i < 9; i++){
			if (mpl4[i] == 1)
				mpl4_ += pow(2, i);
		}
		//Находим значение и ищем его в таблице физических адресов
		physic = r + mpl4_ * 8;
		for (int i = 0; i < m; i++){
			for (int j = 0; j < 2; j++){
				if (physic == paddr_array[i][0]){
					value = paddr_array[i][1];
					cnt_of_phys = 1;
					break;
				}
			}
		}
		if (cnt_of_phys == 0){
			cnt_fault++;
		}
		//Выделяем физический адрес из value
		int value_phys_addr[64];
		if (value % 2 == 0){
			cnt_fault++;
		}
		else{
			for (int i = 0; i < 64; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 0; value != 0; i++){
				value_phys_addr[i] = value % 2;
				value /= 2;
			}
			for (int i = 0; i < 12; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 52; i < 64; i++){
				value_phys_addr[i] = 0;
			}
		}
		value_ = 0;
		for (int i = 12; i < 52; i++){
			if (value_phys_addr[i] == 1)
				value_ += pow(2, i);
		}
		//Выделяем Directory_Ptr
		int directory_ptr[9];
		directory_ptr_ = 0;
		for (int i = 30, j = 0; i <= 38; i++, j++){
			directory_ptr[j] = addr[i];
		}
		for (int i = 0; i < 9; i++){
			if (directory_ptr[i] == 1)
				directory_ptr_ += pow(2, i);
		}
		//Находим значение физического адреса и ищем его в таблице
		physic = value_ + directory_ptr_ * 8;
		cnt_of_phys = 0;
		for (int i = 0; i < m; i++){
			for (int j = 0; j < 2; j++){
				if (physic == paddr_array[i][0]){
					value = paddr_array[i][1];
					cnt_of_phys = 1;
					break;
				}
			}
		}
		if (cnt_of_phys == 0){
			cnt_fault++;
		}
		//Выделяем физичечкий адрес из value
		if (value % 2 == 0){
			cnt_fault++;
		}
		else{
			for (int i = 0; i < 64; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 0; value != 0; i++){
				value_phys_addr[i] = value % 2;
				value /= 2;
			}
			for (int i = 0; i < 12; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 52; i < 64; i++){
				value_phys_addr[i] = 0;
			}
		}
		//Выделяем directory
		value_ = 0;
		//Выделяем физичечкий адрес из value
		for (int i = 12; i < 52; i++){
			if (value_phys_addr[i] == 1)
				value_ += pow(2, i);
		}
		uint64_t directory[9];
		directory_ = 0;
		for (int i = 21, j = 0; i <= 29; i++, j++){
			directory[j] = addr[i];
		}
		for (int i = 0; i < 9; i++){
			if (directory[i] == 1){
				directory_ += pow(2, i);
			}
		}
		physic = value_ + directory_ * 8;
		cnt_of_phys = 0;
		for (int i = 0; i < m; i++){
			for (int j = 0; j < 2; j++){
				if (physic == paddr_array[i][0]){
					value = paddr_array[i][1];
					cnt_of_phys = 1;
					break;
				}
			}
		}
		if (cnt_of_phys == 0){
			cnt_fault++;
		}
		//Выделяем физичечкий адрес из value
		if (value % 2 == 0){
			cnt_fault++;
		}
		else{
			for (int i = 0; i < 64; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 0; value != 0; i++){
				value_phys_addr[i] = value % 2;
				value /= 2;
			}
			for (int i = 0; i < 12; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 52; i < 64; i++){
				value_phys_addr[i] = 0;
			}
		}
		value_ = 0;
		for (int i = 12; i < 52; i++){
			if (value_phys_addr[i] == 1)
				value_ += pow(2, i);
		}
		//Извлекаем table_ptr
		int table[10];
		table_ = 0;
		for (int i = 12, j = 0; i <= 20; i++, j++){
			table[j] = addr[i];
		}
		for (int i = 0; i < 9; i++){
			if (table[i] == 1)
				table_ += pow(2, i);
		}
		physic = value_ +  table_ * 8;
		cnt_of_phys = 0;
		for (int i = 0; i < m; i++){
			for (int j = 0; j < 2; j++){
				if (physic == paddr_array[i][0]){
					value = paddr_array[i][1];
					cnt_of_phys = 1;
					break;
				}
			}
		}
		if (cnt_of_phys == 0){
			cnt_fault++;
		}

		if (value % 2 == 0){
			cnt_fault++;
		}
		else{
			for (int i = 0; i < 64; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 0; value != 0; i++){
				value_phys_addr[i] = value % 2;
				value /= 2;
			}
			for (int i = 0; i < 12; i++){
				value_phys_addr[i] = 0;
			}
			for (int i = 52; i < 64; i++){
				value_phys_addr[i] = 0;
			}
		}
		//Выделяем физичечкий адрес из value
		value_ = 0;
		for (int i = 12; i < 52; i++){
			if (value_phys_addr[i] == 1)
				value_ += pow(2, i);
		}
		int offset[12];
		//Находим оффсет
		offset_ = 0;
		for (int i = 0; i < 12; i++){
			offset[i] = addr[i];
		}
		for (int i = 0; i < 12; i++){
			if (offset[i] == 1){
				offset_ += pow(2, i);
			}
		} ;
		if (cnt_fault != 0)
			fout << "fault" << endl;
		else{
			physic = value_ + offset_;
			fout << physic << endl;
		}
		cnt_fault = 0;
	}
	fout.close();
	return 0;
}
