#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "usbd_cdc_core.h"
#include "util.h"
#include "defines.h"
#include "arm_math.h"

void unwrap(float *p, int N)
 // ported from matlab (Dec 2002)
  {
	int MAX_LENGTH=FFT_LENGTH;
    float dp[MAX_LENGTH];
    float dps[MAX_LENGTH];
    float dp_corr[MAX_LENGTH];
    float cumsum[MAX_LENGTH];
    float M_PI = PI;
    float cutoff = M_PI;               /* default value in matlab */
    int j;

   // incremental phase variation
   // MATLAB: dp = diff(p, 1, 1);
    for (j = 0; j < N-1; j++)
      dp[j] = p[j+1] - p[j];

   // equivalent phase variation in [-pi, pi]
   // MATLAB: dps = mod(dp+dp,2*pi) - pi;
    for (j = 0; j < N-1; j++)
      dps[j] = (dp[j]+M_PI) - floor((dp[j]+M_PI) / (2*M_PI))*(2*M_PI) - M_PI;

   // preserve variation sign for +pi vs. -pi
   // MATLAB: dps(dps==pi & dp>0,:) = pi;
    for (j = 0; j < N-1; j++)
      if ((dps[j] == -M_PI) && (dp[j] > 0))
        dps[j] = M_PI;

   // incremental phase correction
   // MATLAB: dp_corr = dps - dp;
    for (j = 0; j < N-1; j++)
      dp_corr[j] = dps[j] - dp[j];

   // Ignore correction when incremental variation is smaller than cutoff
   // MATLAB: dp_corr(abs(dp)<cutoff,:) = 0;
    for (j = 0; j < N-1; j++)
      if (fabs(dp[j]) < cutoff)
        dp_corr[j] = 0;

   // Find cumulative sum of deltas
   // MATLAB: cumsum = cumsum(dp_corr, 1);
    cumsum[0] = dp_corr[0];
    for (j = 1; j < N-1; j++)
      cumsum[j] = cumsum[j-1] + dp_corr[j];

   // Integrate corrections and add to P to produce smoothed phase values
   // MATLAB: p(2:m,:) = p(2:m,:) + cumsum(dp_corr,1);
    for (j = 1; j < N; j++)
      p[j] += cumsum[j-1];
  }
