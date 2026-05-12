#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <iomanip>

using namespace std;

// ---------------- BUBBLE SORT ----------------

void bubbleSort(vector<int>& arr) {

    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {

        bool swapped = false;

        for (int j = 0; j < n - i - 1; j++) {

            if (arr[j] > arr[j + 1]) {

                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        if (!swapped)
            break;
    }
}

void parallelBubbleSort(vector<int>& arr) {

    int n = arr.size();

    for (int i = 0; i < n; i++) {

        #pragma omp parallel for
        for (int j = i % 2; j < n - 1; j += 2) {

            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

// ---------------- MERGE SORT ----------------

void merge(vector<int>& arr, int left, int mid, int right) {

    vector<int> temp(right - left + 1);

    int i = left;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= right) {

        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];

        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (int i = 0; i < temp.size(); i++)
        arr[left + i] = temp[i];
}

void mergeSort(vector<int>& arr, int left, int right) {

    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    merge(arr, left, mid, right);
}

void parallelMergeSortUtil(vector<int>& arr, int left, int right) {

    if (left >= right)
        return;

    // Small arrays handled sequentially
    if (right - left < 1000) {

        mergeSort(arr, left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    #pragma omp task shared(arr)
    parallelMergeSortUtil(arr, left, mid);

    #pragma omp task shared(arr)
    parallelMergeSortUtil(arr, mid + 1, right);

    #pragma omp taskwait

    merge(arr, left, mid, right);
}

void parallelMergeSort(vector<int>& arr) {

    #pragma omp parallel
    {
        #pragma omp single
        {
            parallelMergeSortUtil(arr, 0, arr.size() - 1);
        }
    }
}

// ---------------- PRINT ARRAY ----------------

void printArray(vector<int>& arr) {

    for (int x : arr)
        cout << x << " ";

    cout << endl;
}

// ---------------- MAIN ----------------

int main() {

    cout << fixed << setprecision(6);

    int n;

    cout << "Enter array size: ";
    cin >> n;

    if (n <= 0) {

        cout << "Invalid array size\n";
        return 0;
    }

    vector<int> arr(n);

    srand(time(0));

    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100000;

    char printChoice;

    cout << "Do you want to print arrays? (y/n): ";
    cin >> printChoice;

    double startTime, endTime;

    // ---------------- SEQUENTIAL BUBBLE SORT ----------------

    vector<int> temp1 = arr;

    startTime = omp_get_wtime();

    bubbleSort(temp1);

    endTime = omp_get_wtime();

    cout << "\nSequential Bubble Sort Time: "
         << (endTime - startTime) * 1000
         << " ms";

    // ---------------- PARALLEL BUBBLE SORT ----------------

    vector<int> temp2 = arr;

    startTime = omp_get_wtime();

    parallelBubbleSort(temp2);

    endTime = omp_get_wtime();

    cout << "\nParallel Bubble Sort Time: "
         << (endTime - startTime) * 1000
         << " ms";

    // ---------------- SEQUENTIAL MERGE SORT ----------------

    vector<int> temp3 = arr;

    startTime = omp_get_wtime();

    mergeSort(temp3, 0, n - 1);

    endTime = omp_get_wtime();

    cout << "\nSequential Merge Sort Time: "
         << (endTime - startTime) * 1000
         << " ms";

    // ---------------- PARALLEL MERGE SORT ----------------

    vector<int> temp4 = arr;

    startTime = omp_get_wtime();

    parallelMergeSort(temp4);

    endTime = omp_get_wtime();

    cout << "\nParallel Merge Sort Time: "
         << (endTime - startTime) * 1000
         << " ms\n";

    // ---------------- PRINT SORTED ARRAYS ----------------

    if (printChoice == 'y' || printChoice == 'Y') {

        cout << "\nBubble Sort Output:\n";
        printArray(temp2);

        cout << "\nMerge Sort Output:\n";
        printArray(temp4);
    }

    return 0;
}