static void remove_file_extension(const char* path, char return_path[260])
{
    int length = length_of(path);

    int index_of_last_period = -1;

    for(int i = length - 1; i >= 0; --i)
    {
        if(path[i] == '.')
        {
            index_of_last_period = i;
            break;
        }
    }

    if(index_of_last_period == -1)
        return;

    int new_length = index_of_last_period;

    for(int i = 0; i < new_length; ++i)
    {
        return_path[i] = path[i];
    }
}

static void get_file_name(const char* path, char return_file_name[260])
{
    int length = length_of(path);

    int index_of_last_slash = -1;

    for(int i = length -1; i >= 0; --i)
    {
        if(path[i] == '\\')
        {
            index_of_last_slash = i;
            break;
        }
    }

    if(index_of_last_slash == -1)
        return;

    for(int i = index_of_last_slash + 1; i < length; ++i)
    {
        return_file_name[i - index_of_last_slash - 1] = path[i];
    }
}

static void reverse(char str[], int length)
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

static char* c_itoa(int num, char* str, int base,int* length)
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
 
	*length = i;

    // Reverse the string
    reverse(str, i);
 
    return str;
}

static char* to_string(int i, int* length)
{
	char* str = (char*)calloc(12,sizeof(char));
	str = c_itoa(i, str, 10, length);
	return str;
}

static int length_of(char* s)
{
    char* p = s;
    int length = 0;

    while(*p)
    {
        ++length;
        ++p;
    }

    return length;
}

static double get_distance(double p0_x, double p0_y, double p1_x,double p1_y)
{
    double x_diff = p1_x - p0_x;
    double y_diff = p1_y - p0_y;

    return sqrt(x_diff*x_diff + y_diff*y_diff);
}

static void C_strcpy(char* src, char* dst)
{
	char* p_dst = dst;

	while (*src)
    {
		*p_dst++ = *src++;
	}
	*p_dst = 0;
}

static BOOL C_atoi(const char* src, int* dst)
{
    // Skip any leading whitespace
	while (*src == ' ' || *src == '\t')
		++src;

	// Determine base and direction
	BOOL is_hex = FALSE;
	BOOL is_negative = FALSE;
	if (*src == '0' && (*(src + 1) == 'x' || *(src + 1) == 'X'))
	{
		is_hex = TRUE;
		src += 2;	// move pointer past '0x' part of hex number
	}
	else if (*src == '-')
	{
		is_negative = TRUE;
		++src;		// move pointer past '-' part of decimal number
	}

	int result = 0;

	if (is_hex)
	{
		// base 16: e.g. 0xAb09
		while (*src)
		{
			if ((*src >= '0' && *src <= '9'))
				result += (*src++ - '0');
			else if (*src >= 'A' && *src <= 'F')
				result += (*src++ - 'A' + 10);
			else if (*src >= 'a' && *src <= 'f')
				result += (*src++ - 'a' + 10);
			else
				return FALSE;
			
			if (*src) result *= 16;
		}
	}
	else
	{
		// base 10: e.g. 1037, -24
		while (*src >= '0' && *src <= '9')
		{
			result += (*src++ - '0');
			if(*src) result *= 10;
		}

		if (is_negative)
			result *= -1;
	}

	*dst = result;

	return TRUE;
}

static BOOL str_contains(char* base, char* search_pattern)
{
    char* b_p = base;
    char* p_p = search_pattern;

    while(*b_p)
    {
        if(*b_p == *p_p)
        {
            char* bb_p = b_p;
            BOOL match = TRUE;
            int pattern_length = 0;

            while(*p_p)
            {
                if(*bb_p != *p_p)
                    match = FALSE;
                
                ++p_p;
                ++bb_p;
                ++pattern_length;
            }

            p_p = search_pattern;

            if(match)
            {
                return TRUE;
            }
        }

        ++b_p;
    }

    return FALSE;
}

static void str_replace(char* base,int base_length, char* search_pattern, char* replacement, char* ret_string)
{
    int  ret_string_i = 0;

    char* b_p = base;
    char* p_p = search_pattern;
    char* r_p = replacement;

    while(*b_p)
    {
        if(*b_p == *p_p)
        {
            char* bb_p = b_p;
            BOOL match = TRUE;
            int pattern_length = 0;

            while(*p_p)
            {
                if(*bb_p != *p_p)
                    match = FALSE;
                
                ++p_p;
                ++bb_p;
                ++pattern_length;
            }

            if(match)
            {
                // replace search_pattern with replacement in base
                while(*r_p)
                    ret_string[ret_string_i++] = *r_p++;

                //reset pointers
                b_p += pattern_length;
                p_p = search_pattern;
                r_p = replacement;
            }
        }

        ret_string[ret_string_i++] = *b_p++;
    }
}

