void SPI_MasterInit(void);
void SPI_MasterTransmit(long data);
void SPI_MasterStop();
void SetPot(int Select,int Data,int Command);
void SPI_SlaveUnselect(int Select);
void SPI_SlaveSelect(int Select);
