
#ifndef _USER_MB_M_APP_
#define _USER_MB_M_APP_

#ifdef __cplusplus
extern "C" {
#endif  
typedef enum _MB_REG_M
{
  REG_M_W_CNTRL   = 0 ,
  REG_M_R_STATUS  ,
  
  REG_M_LAST
  
}eMBRegM_t;

void UpDateReadRgM( eMBRegM_t numRg );
void UpDateWriteRgM( eMBRegM_t numRg );

#ifdef __cplusplus
}
#endif  
#endif
  
  