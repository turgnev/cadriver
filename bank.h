struct coin_t
{
	int value;
	int pulse;
};

struct bank_t
{
	struct coin_t coin[4];
	int number[4];
};

static struct bank_t bank;

void init_bank(struct bank_t *b)
{
	int i;
	for (i=0; i<4; i++)
		b->number[i] = 0;
		
	b->coin[0].value = 1;
	b->coin[0].pulse = 1;
	
	b->coin[1].value = 2;
	b->coin[1].pulse = 2;
	
	b->coin[2].value = 5;
	b->coin[2].pulse = 5;
	
	b->coin[3].value = 10;
	b->coin[3].pulse = 10;				
}

void clear_bank(struct bank_t *b)
{
	int i;
	for (i=0; i<4; i++)
		b->number[i] = 0;				
}

