extern char InitializeUart(const long int baudrate);
extern void WriteToUart(char data);
extern char IsUartTxEmpty();
extern void WriteTextToUart(char *text);
extern char IsUartDataReady();
extern char ReadUartData();
extern void ReadUartText(char *Output, unsigned int length);
