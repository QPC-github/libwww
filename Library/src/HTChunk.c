/*								      HTChunk.c
**	CHUNK HANDLING:	FLEXIBLE ARRAYS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** history:	AL, HF	28 Apr 94, Now chunk->data is filled by '\0' so
**			that the string is terminated at any time. That makes
**			HTChunk_terminate not needed any more, but never mind.
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTChunk.h"				         /* Implemented here */

/*	Create a chunk with a certain allocation unit
**	--------------
*/
PUBLIC HTChunk * HTChunk_new (int grow)
{
    HTChunk * ch = (HTChunk *) calloc(1, sizeof(HTChunk));
    if (ch == NULL) outofmem(__FILE__, "cretion of chunk");
    ch->growby = grow;
    return ch;
}


/*	Clear a chunk of all data
**	--------------------------
*/
PUBLIC void HTChunk_clear (HTChunk * ch)
{
    if (ch->data) {
	free(ch->data);
	ch->data = 0;
    }
    ch->size = 0;
    ch->allocated = 0;
}


/*	Free a chunk
**	------------
*/
PUBLIC void HTChunk_delete (HTChunk * ch)
{
    if (ch->data) free(ch->data);
    free(ch);
}


/*	Append a character
**	------------------
*/
PUBLIC void HTChunk_putc (HTChunk * ch, char c)
{
    if (ch->size >= ch->allocated-1) {
	if (ch->data) {
	    ch->data = (char *) realloc(ch->data, ch->allocated + ch->growby);
	    memset((void *) (ch->data + ch->allocated), '\0', ch->growby);
	} else {
	    ch->data = (char *) calloc(1, ch->allocated + ch->growby);
	}
	ch->allocated = ch->allocated + ch->growby;
	if (!ch->data) outofmem(__FILE__, "HTChunk_putc");
    }
    ch->data[ch->size++] = c;
}


/*	Ensure a certain size
**	---------------------
*/
PUBLIC void HTChunk_ensure (HTChunk * ch, int needed)
{
    if (needed <= ch->allocated) return;
    ch->allocated = needed-1 - ((needed-1) % ch->growby)
    			     + ch->growby; /* Round up */
    ch->data = ch->data ? (char *)realloc(ch->data, ch->allocated)
			: (char *)malloc(ch->allocated);
    if (ch->data == NULL) outofmem(__FILE__, "HTChunk_ensure");
}


/*	Terminate a chunk
**	-----------------
*/
PUBLIC void HTChunk_terminate (HTChunk * ch)
{
    HTChunk_putc(ch, (char)0);
}


/*	Append a string
**	---------------
*/
PUBLIC void HTChunk_puts (HTChunk * ch, CONST char * s)
{
    CONST char * p;
    for (p=s; *p; p++)
        HTChunk_putc(ch, *p);
}

/*	Append a block
**	---------------
*/
PUBLIC void HTChunk_putb (HTChunk * ch, CONST char *block, int len)
{
    int i;
    for (i = 0; i < len; i++)
        HTChunk_putc(ch, block[i]);
}
