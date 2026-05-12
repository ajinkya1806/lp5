#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <omp.h>
#include <iomanip>

using namespace std;

// ---------------- SEQUENTIAL OPERATIONS ----------------

int sequentialMin(vector<int>& arr) {

    int mini = INT_MAX;

    for (int x : arr)
        mini = min(mini, x);

    return mini;
}

int sequentialMax(vector<int>& arr) {

    int maxi = INT_MIN;

    for (int x : arr)
        maxi = max(maxi, x);

    return maxi;
}

long long sequentialSum(vector<int>& arr) {

    long long sum = 0;

    for (int x : arr)
        sum += x;

    return sum;
}

double sequentialAverage(vector<int>& arr) {

    return (double)sequentialSum(arr) / arr.size();
}

// ---------------- PARALLEL OPERATIONS ----------------

int parallelMin(vector<int>& arr) {

    int mini = INT_MAX;

    #pragma omp parallel for reduction(min:mini)
    for (int i = 0; i < arr.size(); i++) {

        if (arr[i] < mini)
            mini = arr[i];
    }

    return mini;
}

int parallelMax(vector<int>& arr) {

    int maxi = INT_MIN;

    #pragma omp parallel for reduction(max:maxi)
    for (int i = 0; i < arr.size(); i++) {

        if (arr[i] > maxi)
            maxi = arr[i];
    }

    return maxi;
}

long long parallelSum(vector<int>& arr) {

    long long sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < arr.size(); i++) {

        sum += arr[i];
    }

    return sum;
}

double parallelAverage(vector<int>& arr) {

    return (double)parallelSum(arr) / arr.size();
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

    double startTime, endTime;

    // ---------------- MINIMUM ----------------

    startTime = omp_get_wtime();

    int seqMin = sequentialMin(arr);

    endTime = omp_get_wtime();

    cout << "\nSequential Min: " << seqMin;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    startTime = omp_get_wtime();

    int parMin = parallelMin(arr);

    endTime = omp_get_wtime();

    cout << "\nParallel Min: " << parMin;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    // ---------------- MAXIMUM ----------------

    startTime = omp_get_wtime();

    int seqMax = sequentialMax(arr);

    endTime = omp_get_wtime();

    cout << "\nSequential Max: " << seqMax;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    startTime = omp_get_wtime();

    int parMax = parallelMax(arr);

    endTime = omp_get_wtime();

    cout << "\nParallel Max: " << parMax;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    // ---------------- SUM ----------------

    startTime = omp_get_wtime();

    long long seqSum = sequentialSum(arr);

    endTime = omp_get_wtime();

    cout << "\nSequential Sum: " << seqSum;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    startTime = omp_get_wtime();

    long long parSum = parallelSum(arr);

    endTime = omp_get_wtime();

    cout << "\nParallel Sum: " << parSum;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    // ---------------- AVERAGE ----------------

    startTime = omp_get_wtime();

    double seqAvg = sequentialAverage(arr);

    endTime = omp_get_wtime();

    cout << "\nSequential Average: " << seqAvg;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    startTime = omp_get_wtime();

    double parAvg = parallelAverage(arr);

    endTime = omp_get_wtime();

    cout << "\nParallel Average: " << parAvg;
    cout << "\nTime: "
         << (endTime - startTime) * 1000
         << " ms\n";

    return 0;
}