//Более новый комментарий
//Новый комментарий
// Функция сортировки пузырьком
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                // Меняем элементы местами
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

int main() {
	int a[] = {4,2};
	int n = sizeof(a) / sizeof(a[0]);
	bubbleSort(a,3);

	return 0;
}
