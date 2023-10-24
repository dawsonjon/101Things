#ifndef EQ_H
#define EQ_H

//IIR filters calculated using http://jaggedplanet.com/iir/iir-explorer.asp

//0.25 -> 0.5
class cfilter5
{
  #define NBQ 2
  float biquada[4]={0.455324337536242,0.3035929544357675,0.05066656638882013,0.2191779239786963};
  float biquadb[4]={1,-2,1,-2};
  float gain=16.70753810558651;
  float xyv[9]={0,0,0,0,0,0,0,0,0};

  public:
  float applyfilter(float v)
  {
    int i,b,xp=0,yp=3,bqp=0;
    float out=v/gain;
    for (i=8; i>0; i--) {xyv[i]=xyv[i-1];}
    for (b=0; b<NBQ; b++)
    {
      int len=2;
      xyv[xp]=out;
      for(i=0; i<len; i++) { out+=xyv[xp+len-i]*biquadb[bqp+i]-xyv[yp+len-i]*biquada[bqp+i]; }
      bqp+=len;
      xyv[yp]=out;
      xp=yp; yp+=len+1;
    }
    return out;
  }
};

//0.125 -> 0.25
class cfilter4
{
  #define NBQ 4
  float biquada[8]={0.7579176129404435,-1.0890974486597524,0.4184575191541888,-0.634176665935791,0.3718991286657101,-0.06464236918798119,0.7063160558870645,0.3302953104371898};
  float biquadb[8]={1,-2,1,-2,1,2,1,2};
  float gain=59.89752224470513;
  float xyv[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  public:
  float applyfilter(float v)
  {
    int i,b,xp=0,yp=3,bqp=0;
    float out=v/gain;
    for (i=14; i>0; i--) {xyv[i]=xyv[i-1];}
    for (b=0; b<NBQ; b++)
    {
      int len=2;
      xyv[xp]=out;
      for(i=0; i<len; i++) { out+=xyv[xp+len-i]*biquadb[bqp+i]-xyv[yp+len-i]*biquada[bqp+i]; }
      bqp+=len;
      xyv[yp]=out;
      xp=yp; yp+=len+1;
    }
    return out;
  }
};

//0.0625 -> 0.125
class cfilter3
{
  #define NBQ 4
  float biquada[8]={0.8837957282542943,-1.6943599206515976,0.6857477951989787,-1.4396361389188566,0.6115117599816042,-1.1922405965447527,0.8035759260859956,-1.1472434046685962};
  float biquadb[8]={1,-2,1,-2,1,2,1,2};
  float gain=616.703871384361;
  float xyv[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  public:
  float applyfilter(float v)
  {
    int i,b,xp=0,yp=3,bqp=0;
    float out=v/gain;
    for (i=14; i>0; i--) {xyv[i]=xyv[i-1];}
    for (b=0; b<NBQ; b++)
    {
      int len=2;
      xyv[xp]=out;
      for(i=0; i<len; i++) { out+=xyv[xp+len-i]*biquadb[bqp+i]-xyv[yp+len-i]*biquada[bqp+i]; }
      bqp+=len;
      xyv[yp]=out;
      xp=yp; yp+=len+1;
    }
    return out;
  }
};

//0.03125 -> 0.0625
class cfilter2
{
  #define NBQ 4
  float biquada[8]={0.9408096822533009,-1.8925378666070687,0.8295282462723398,-1.7628884919432277,0.7775711951153536,-1.6589737681551742,0.8884894457295361,-1.7057663453697884};
  float biquadb[8]={1,-2,1,-2,1,2,1,2};
  float gain=6924.3381047822995;
  float xyv[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  public:
  float applyfilter(float v)
  {
    int i,b,xp=0,yp=3,bqp=0;
    float out=v/gain;
    for (i=14; i>0; i--) {xyv[i]=xyv[i-1];}
    for (b=0; b<NBQ; b++)
    {
      int len=2;
      xyv[xp]=out;
      for(i=0; i<len; i++) { out+=xyv[xp+len-i]*biquadb[bqp+i]-xyv[yp+len-i]*biquada[bqp+i]; }
      bqp+=len;
      xyv[yp]=out;
      xp=yp; yp+=len+1;
    }
    return out;
  }
};

//0 -> 0.03125
class cfilter1
{
  #define NBQ 2
  float biquada[4]={0.8441839938056334,-1.7986744459409212,0.6611598072837459,-1.6201668087464842};
  float biquadb[4]={1,2,1,2};
  float gain=8576.454240144143;
  float xyv[9]={0,0,0,0,0,0,0,0,0};

  public:
  float applyfilter(float v)
  {
    int i,b,xp=0,yp=3,bqp=0;
    float out=v/gain;
    for (i=8; i>0; i--) {xyv[i]=xyv[i-1];}
    for (b=0; b<NBQ; b++)
    {
      int len=2;
      xyv[xp]=out;
      for(i=0; i<len; i++) { out+=xyv[xp+len-i]*biquadb[bqp+i]-xyv[yp+len-i]*biquada[bqp+i]; }
      bqp+=len;
      xyv[yp]=out;
      xp=yp; yp+=len+1;
    }
    return out;
  }
};

class eq
{
  cfilter1 filter1;
  cfilter2 filter2;
  cfilter3 filter3;
  cfilter4 filter4;
  cfilter5 filter5;

  public:
  float applyfilter(float sample, float gains[5])
  {
    float ret;
    ret = filter1.applyfilter(sample)*gains[0];
    ret += filter2.applyfilter(sample)*gains[1];
    ret += filter3.applyfilter(sample)*gains[2];
    ret += filter4.applyfilter(sample)*gains[3];
    ret += filter5.applyfilter(sample)*gains[4];
    return ret;
  }
};

#endif
