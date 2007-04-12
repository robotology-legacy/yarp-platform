function dataOut = lmTrimDataT(AG, dataIn, t0, t1);
% Info:  trims dataIn and fixes dataOut
% Usage: dataOut = mFixData (AG, dataIn)
% Where: AG        profile
%        dataIn    input data
%        t0        from time t0...
%        t1        ... to time t1, according 
%                  to AG.pars.tu time unit

s0 = lmT2S(AG, t0);
s1 = lmT2S(AG, t1);

printf ('[lmTrimDataT] Trimming data: %d to %d %s\n', t0, t1, AG.pars.tu);
dataOut = lmTrimDataS(AG, dataIn, s0, s1);
