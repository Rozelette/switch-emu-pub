int sum(int i) {
    if (i == 0) {
        return 0;
    } else {
        return i + sum(i - 1);
    }
}

int main(void) {
    int a = 5;

    return sum(a);
}