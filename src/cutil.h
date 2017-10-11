void reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end)
	{
        // swap
        char t = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = t;

		start++;
		end--;
	}
}

char* c_itoa(int num, char* str, int base)
{
    int i = 0;
    BOOL isNegative = FALSE;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = TRUE;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}
char* to_string(int i)
{
	char* str = (char*)calloc(12,sizeof(char));
	str = c_itoa(i, str, 10);
	return str;
}

double get_distance(double p0_x, double p0_y, double p1_x,double p1_y)
{
    double x_diff = p1_x - p0_x;
    double y_diff = p1_y - p0_y;

    return sqrt(x_diff*x_diff + y_diff*y_diff);
}

