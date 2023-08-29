/*******************************************************************************
*
* Copyright (C) 2010 Sharopin Yuri
*
* File              : Filters.c
* Compiler          : IAR EWAARM 5.xx
* Version           : 0.0
* Last modified     : 09.07.2010
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: STM32F103 @ (Xtal = MHz | CPUclk = MHz)
* Description       : Модуль цифровых фильтров
*                   :

* Hardware          : .sch .pcb
*
********************************************************************************/

/*===============================[ SPECIAL ]===================================*/
//  Условная компиляция

/*===============================[ IMPORT DECLARATIONS ]=======================*/
// Все системные, библиотечные и заголовочные файлы
/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> //  MCS
/*-------------------------------[ Platform level    ]-------------------------*/
//#include   <> //  IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
#include    <math.h>      //
/*-------------------------------[ Application Level ]-------------------------*/
#include "main.h"
#include   "Filters.h"
#include   "Sorting.h"

/*===============================[ PRIVATE CONSTANTS ]=========================*/
// Все частные #defines и constants должны быть объявлены в данном разделе.
//#define   FILTER_SIZE     19 // - для Gauss1 // 8
/*===============================[ PRIVATE TYPES ]=============================*/
// Все частные типы, которые используются в данном исходном файле, должны быть объявлены в данном разделе.
/*===============================[ PRIVATE VARIABLES ]=========================*/
// В этом разделе объявляются все частные переменные.

float Buf_H [ FILTER_SIZE ];    // импульсная характеристика фильтра
float Buf_S [ FILTER_SIZE ];    // FIFO-буфер сигнала
int   iSignal = 0;              // индекс буфера сигнала

/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Butterworth
Filter order: 3
Sampling Frequency: 8 KHz
Cut Frequency: 0.001000 KHz
Coefficents Quantization: float
*****************************************************************/
const
float FIRCoef[ ] =
{
        0.05499466358068422900,
        0.05637497598671972800,
        0.05798202846952028900,
        0.05992037088847492600,
        0.06238451018555148100,
        0.06580295043481729200,
        0.07146245873743400900,
        0.08837758594257574800,
        0.07146245873743400900,
        0.06580295043481729200,
        0.06238451018555148100,
        0.05992037088847492600,
        0.05798202846952028900,
        0.05637497598671972800,
        0.05499466358068422900,
        0.05377849749102030200
};
/*
{
    0.05044261839203362900,
    0.05261046499797739800,
    0.05515894912976825000,
    0.05825907921539941200,
    0.06222870783459746900,
    0.06776757429717315800,
    0.07697592851658106900,
    0.10456044720389891000,
    0.07697592851658106900,
    0.06776757429717315800,
    0.06222870783459746900,
    0.05825907921539941200,
    0.05515894912976825000,
    0.05261046499797739800,
    0.05044261839203362900,
    0.04855290802904020000
};
*/


const uint16_t SizeFIR = sizeof( FIRCoef );

const
float Gauss1[ ] =               // первая производная функции Гаусса
{	                        // для выделения откликов на шев типа ступенька
  0.5975	,               // предварительные вычисления в process.mcd
  0.6065	,
  0.5967	,
  0.5661	,
  0.5141	,
  0.4412	,
  0.3495	,
  0.2423	,
   0.124	,
       0	,
  -0.124	,
 -0.2423	,
 -0.3495	,
 -0.4412	,
 -0.5141	,
 -0.5661	,
 -0.5967	,
 -0.6065	,
 -0.5975	,
};

const uint16_t SizeGF = sizeof( Gauss1 );

/*===============================[ PUBLIC  VARIABLES ]=========================*/
// Все глобальные переменные должны быть заданы в этом разделе.
/*===============================[ PRIVATE PSEUDO FUNCTIONS ]==================*/
// Все макросы должны быть объявлены в этом разделе.
/*===============================[ PRIVATE FUNCTIONS ]=========================*/
// Все частные функции, используемые в данном исходном файле.
/*===============================[ PUBLIC FUNCTIONS ]==========================*/
// Все экспортируемые фукнции.

//-------------------------- [ Пороговый фильтр ] --------------------------
/*! \ingroup
* Function Name  : ThresholdFilter()
* \brief         : Пороговый фильтр. Ограничение одиночных сверх-больших выбрасов.
* \param Input   : None
* \param Output  : None
* \return        : None
*/
//------------------------------------------------------------------------------
void  ThresholdFilter( )
{

} // ThresholdFilter( )

//-------------------------- [  ] --------------------------
/*! \ingroup
* Function Name  : Init_LF_Filter()
* \brief         : Вычисление импульсной харатеристики фильтра
* \param Input   : None
* \param Output  : None
* \return        : None
*/
//------------------------------------------------------------------------------
void Init_LF_Filter( )
{
  /*  Фильтр любезно предоставленный и написанный С.П. Недяком по книге
      Гихман, Скороход. Введение в торию случайных процессов - Наука. 1977.
  */
  //float af    = 2 * PI / ( FILTER_SIZE - 1 );
  Buf_H[ 0 ]  = 0;//af / PI;
  Buf_S[ 0 ]  = 0;

  for ( int t = 0; t < FILTER_SIZE; t++ ) //t=1
  {
     Buf_H[ t ] = Gauss1[ t ];//sin( af * t ) / PI / t;
     Buf_S[ t ] = 0;
  } //t

  iSignal = 0;
} // Init_LF_Filter( )

//-------------------------- [ НЧ-фильтр ] --------------------------
/*! \ingroup
* Function Name  : LF_filter()
* \brief         : НЧ-фильтр. Вызывается в цикле для каждого отчета сигнала
* \param Input   : None
* \param Output  : None
* \return        : None
*/
//------------------------------------------------------------------------------
float LF_filter( float Xn )
{
  int  tau, t;

  float Summa = 0;                // Сумма

  Buf_S[ iSignal ] = Xn;

  tau  = iSignal;

  for( t = 0; t < FILTER_SIZE; t++ )
  {
    Summa += Buf_H[ t ] * Buf_S[ tau ];
    tau--;
    if ( tau < 0 ) tau = FILTER_SIZE - 1;
  }//t

  iSignal ++;
  if ( iSignal > FILTER_SIZE - 1 ) iSignal = 0;

  return Summa;
} // LF_filter()()

/*
 * Свертка со смещением на половину отсчетов фильтра
   x - входной массив целых чисел
   N - число отсчетов в x
   h - импульсная характеристика или сигнал-образ
   M - число отсчетов в h
return: указатель на масив результата. Размер выходной последовательности равен M + N - 1
*/
Signal_t* Convolution( Signal_t* result, Signal_t* x, uint16_t N, const float* h, uint16_t M )
{

  //gServDbg.bit.Filtering = 1; // установить бит, чтобы затем очистить память

  // double * result = new double[N + M - 1];
  //Signal_t* result = 0;
  //if ( result = (Signal_t*)malloc( sizeof(Signal_t) * (N + M - 1) ))
  //{
    // обнулить первые M чисел. т.к. в начале сигнала какие-то всплеске
    for ( uint16_t i = 0; i < (N + M - 1); i++ )  result[ i ] = 0;
    //memset( result, 0, sizeof(Signal_t) * (N + M - 1) );
    Signal_t tmpS; float tmpH;
    for (uint16_t i = 0; i < N; ++i)
    {
      tmpS = x[i];
      for (uint16_t j = 0; j < M; ++j)
      {
        tmpH = h[j];
        result[i + j] += tmpS * tmpH;
      }
    }
    //memset( result, 0, sizeof(Signal_t) * M );

  int16_t shift   = M / 2;
  for ( uint16_t i = 0; i < N; i++ )
  {
    // смещаем результат, если применили фильтр
    if ( i < (N - shift) )
      result[ i ] =  result[ i + shift ];
    else
      result[ i ] =  result[ i ];

    // Убираем граничный эффект фильтра
    if ( i < M )
      result[ i ] = result[ M + shift ];
    if (i > (N - shift) )
      result[ i ] = result[ i - shift ];
  }

    return result;
  //}
  //else return NULL;
} // Convolution( )

float* Convolutionf( float* result, float* x,  uint16_t const N, const float* h, uint16_t M )
{

  //gServDbg.bit.Filtering = 1; // установить бит, чтобы затем очистить память

  // double * result = new double[N + M - 1];
  //Signal_t* result = 0;
  //if ( result = (Signal_t*)malloc( sizeof(Signal_t) * (N + M - 1) ))
  //{
    // обнулить первые M чисел. т.к. в начале сигнала какие-то всплеске
    //for ( uint16_t i = 0; i < M; i++ )  x[i] = 0;
    memset( result, 0, sizeof(Signal_t) * (N + M - 1) );
    float tmpS; float tmpH;
    for (uint16_t i = 0; i < N; ++i)
    {
      tmpS = x[i];
      for (uint16_t j = 0; j < M; ++j)
      {
        tmpH = h[j];
        result[i + j] += tmpS * tmpH;
      }
    }
    //memset( result, 0, sizeof(Signal_t) * M );

  int16_t shift   = M / 2;
  for ( uint16_t i = 0; i < N; i++ )
  {
    // смещаем результат, если применили фильтр
    if ( i < (N - shift) )
      result[ i ] =  result[ i + shift ];
    else
      result[ i ] =  result[ i ];

    // Убираем граничный эффект фильтра
    if ( i < M )
      result[ i ] = result[ M + shift ];
    if ( i > ( N - shift) )
      result[ i ] = result[ i - shift ];
  }

    return result;
  //}
  //else return NULL;
} // Convolution( )

//-------------------------- [ НЧ-фильтр ] --------------------------
/*! \ingroup
* Function Name  : LPFilter()
* \brief         : НЧ-фильтр. Вызывается для всего сигнала целиком
* \param Input   : None
* \param Output  : None
* \return        : None
*/
//------------------------------------------------------------------------------
void LPFilter( uint16_t* RawSignal, uint16_t Len )
{
  for ( int t = 1; t < FILTER_SIZE; t++ )
  {
    Buf_S[ t ] = RawSignal[ t ]; // заполнить буфер сигнала
  } //for( t )
  for (uint16_t i = 1; i < Len; i++ )
  {
    RawSignal[ i-1 ] = (uint16_t)LF_filter( RawSignal[ i ] );
    //RawSignal[ i-1 ] = fir( RawSignal[ i ] );
  }
} // LPFilter()


//-------------------------- [  ] --------------------------
/*! \ingroup
* Function Name  :
* \brief         : Пороговая фильтрация - отбрасываем +-30% от среднего по первой производной
                 : Пороговая фильтрация - выкинем резкие всплески
* \param Input   : None
* \param Output  : None
* \return        : None
*/
//------------------------------------------------------------------------------
#define SIZE_WIND     8
void Threshold( uint16_t* RawSignal, uint16_t Len )
{
  double sum = 0;
  for (int i = 0; i < Len - 1 - SIZE_WIND; i ++ )
  {
    // среднее в окне SIZE_WIND
    for (int j = i; j < i + SIZE_WIND; j ++ )
    {
      sum += RawSignal[ j ];
    }
    sum /= SIZE_WIND;
    if ( abs_i( RawSignal[ i ] - RawSignal[ i + 1 ] )  > sum / 7 )
    {
      RawSignal[ i + 1 ] = RawSignal[ i ];
    }
    sum = 0;
  }
//  return 0;
} // Threshold( )


/*! \ingroup 	: Filters
* \brief        : Медианная фильтрация сигнала
* \param Input  : Res - результат, size - длина массива, win - размер окна
* \param Output :
* \return
*/
void  MedianFiltr( Signal_t* Res, Signal_t* Signal, uint16_t size, uint16_t win )
{
  Signal_t rank[ 16 ];                           // массив для сортировки элементов в окне

  if ((win%2 == 0) || (win > 15))
  {
    WR_DEBUG("Неверно задан размер окна. должно быть Нечетным и меньше 15.");
    return;
  }

  for ( uint16_t j = 0; j <= size - win; j++ )
  {
    uint16_t 	k = 0;  		 // индекс массива для сортировки
    uint16_t 	d = win + 1;		 // индекс цикла сортировки
    for (uint16_t m = j; m < win + j; m++)
    {
            rank[ k ] = Signal[ m ];
            k++;
    };
    // сортируем массив TODO qsort() не работает !!? или я чего-то не понимаю... TODO переписать сортировку
    do{
      k = 1;
      if ( d ) d--; else
        break;
      for (uint16_t m = 0; m < d; m++ )
        if ( rank[ m ] > rank[ m + 1 ] )
        {
          Signal_t tmp = rank[ m + 1 ];
          rank[ m + 1] = rank[ m ];
          rank[ m ] = tmp;
          k = 0;
        }
    }
    while ( k == 0 );
    Res[ j ] = rank[ win/2 ];          // берем середину
  }
  // учтем граничный эффект
  for (uint16_t i =  size - win + 1; i < size; i++) Res[ i ] = Res[ size - win ];
}// конец Функции фильтрации


/**
  * @brief   Get the average of N-X ADC samples
  * AN4073 Averaging of N-X ADC samples: source code
  * @param Number of ADC samples to be averaged
  * @param Number of ADC samples to be averaged
  * @retval The average value
  */
int32_t AverageN_DeleteX( uint16_t* adc_sample, uint16_t N , uint16_t X )
{
  uint32_t avg_sample 	   = 0x00;
  uint8_t  index	   = 0x00;

  if ( N < X ) {

    return -1;
  }else;

  if ( X % 2 ) return -2; // требуем четности окна

  /* Sort the N-X ADC samples */
  Sort_tab( adc_sample, N );

  /* Add the N ADC samples */
  for ( index = X/2; index < N - X/2; index++)
  {
    avg_sample += adc_sample[ index ];
  }

  /* Compute the average of N-X ADC sample */
  avg_sample /= N - X + 1; // !!! + 1 здесь лишний для четного X!!! // BUG!!! для красноярска пока оставляем что бы не исправлять коэф.
                           // Уточнить поправку в коэффициентах

  /* Return average value */
  return avg_sample;
} // AverageN_DeleteX()

float32_t AverageN_DeleteX_f( float32_t* adc_sample, uint16_t N , uint16_t X )
{
  float32_t avg_sample 	   = 0x00;
  uint8_t   index	   = 0x00;

  if ( N < X ) {

    return -1;
  }else;

  if ( X % 2 ) return -2; // требуем четности окна

  /* Sort the N-X ADC samples */
  Sort_tab_f( adc_sample, N );

  /* Add the N ADC samples */
  ///StatHV.min = adc_sample[ X/2     ];  TODO вернуть размах !!!
  ///StatHV.max = adc_sample[ N - X/2 ];

  for ( index = X/2; index < N - X/2; index++)
  {
    avg_sample += adc_sample[ index ];
  }

  /* Compute the average of N-X ADC sample */
  avg_sample /= N - X;/// + 1; // !!! + 1 здесь лишний для четного X!!!

  /* Return average value */
  return avg_sample;
} // AverageN_DeleteX_f()

int32_t abs_i ( int32_t value )
{
  if ( value < 0 ) value *= -1;

  return value;
} // abs_i( )

/** (END OF FILE  : Filters.c)
*******************************/
