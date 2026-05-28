struct Point {
    int x;
    double y;
};

int global_counter;
double pi;

int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
int min(int a,int b){
    int x;

  
        int x;
    
}
void main() {
    int i;
    int limit;
    int arr[5];
    struct Point p;
    double result;

    limit = 5;
    i = 0;
    pi = 3.14;
    global_counter = 0;

    while (i < limit) {
        arr[i] = i * 2;
        i = i + 1;
        break;
    }

    for (i = 0; i < limit; i = i + 1) {
        if (arr[i] == 4 || arr[i] != 0) {
            p.x = arr[i];
            global_counter = global_counter + 1;
        }
    }

    p.y = pi;
    result = max(p.x, 10);

    if (result >= 10.0 && global_counter < 100) {
    }

    return;
}