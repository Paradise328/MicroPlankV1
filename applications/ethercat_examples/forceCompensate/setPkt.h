extern unsigned char abSendData[128];
extern unsigned char abRecvData[128];
extern CIFX_PACKET tSendPkt ;
extern CIFX_PACKET tRecvPkt ;

int SendDataOffset=30;
int RecvDataOffset=22;

int update_speed(int joint,long int target_velocity){

  int p1,p2,p3,p4;
  long int target_velocity_abs=target_velocity;

  if(target_velocity<0){
      target_velocity_abs=-target_velocity;
  }

  p1 = target_velocity_abs%256;
  p2 = target_velocity_abs/256%256;
  p3 = target_velocity_abs/256/256%256;
  p4 = target_velocity_abs/256/256/256%256;

  if(target_velocity<0){
      p1=255-p1;
      p2=255-p2;
      p3=255-p3;
      p4=255-p4;
  }

  abSendData[10+SendDataOffset*(joint-1)]=p1;
  abSendData[11+SendDataOffset*(joint-1)]=p2;
  abSendData[12+SendDataOffset*(joint-1)]=p3;
  abSendData[13+SendDataOffset*(joint-1)]=p4;

  return 1;
}