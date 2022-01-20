

int pow (int base, int exponent) {
    for(int i = 1; i < exponent; i++) 
        base *= base;
    return base;
}