void msg(uint8_t);
void emit(uint8_t);
void crlf(void);
void dump8(uint8_t);
void dump16(uint16_t);
void dump32(uint32_t);

#define	mStart	0
#define	mEnd 1
#define	m_ts 2
#define	m_nPage 3
#define	m_writePage 4
#define	m_erasePage 5
#define	m_prev 6
