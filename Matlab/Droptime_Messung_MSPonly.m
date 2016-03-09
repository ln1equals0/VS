clear all;
%Matlab-Skript zum Droptime MSP430 Board
%Dieses Skript fordert fordert fortlaufend Daten vom Mikrocontroller an,
%indem es das Zeichen 'm' über UART sendet. 
%Autor: Christian Harira mit Unterstützung von Stephan Schultheis
%       Modified for MSP430 Droptime Board by D. Eichel 08/2011
%
%------------------------COM-Port einstellen ------------------------------
s = serial('COM11',...                       % ggf. COM Port ändern !!!!!
           'BaudRate', 500000,...
           'Parity', 'none',...
           'DataBits', 8,...
           'StopBits', 1,...
           'InputBufferSize', 2048,...
           'OutputBufferSize', 2048,...
           'Timeout', 2);
     
% Messung (Matlab zählt ab 1)

      ADC=[];   % Byte1 Lowbyte, Byte 2 Highbyte
                % Empfangene Werte von ADC,
                % zusätzlich im Highbyte: Bit8: Vom µC generierter Trigger,
                % Bit7: Teilchen Lichtschr., Bit6: Backen Lichtschr.
      Byte3=[]; % Empfangene Werte von ASCII 059        
                
      ArraySize = 100*128; % sollte eine Zahl, die durch 128 teilbar ist sein. Nicht größer als 896000!
      ArraySize = ArraySize-mod(ArraySize,128); % falls doch mal einer nicht rechnen kann      
      ADCVals=zeros(ArraySize,1);
      Byte1Vals=zeros(ArraySize,1);
      Byte3Vals=zeros(ArraySize,1);
      LichtVals=zeros(ArraySize,1);
      ReflVals=zeros(ArraySize,1);
      TriggerVals=zeros(ArraySize,1);
      WaageVals=zeros(ArraySize,1);
      
      usart_frame_size=510; %(8Bit = 1Byte)
                            % Grösse des zu empfangenden Pakets muss durch 3 teilbar sein!!

      fopen(s);              % Com-Port öffnen
      fprintf(s,'%c','s');   % Start der Messung durch Senden von 's'

    ret_val = uint16(fread(s,usart_frame_size,'char')); % 1st frame contains
                                                        % sensor setup
    SensorConfig = char(ret_val(1:1:usart_frame_size))';

    
      % ..-mal Datenframe empfangen
    for a = 1:ArraySize/128 - 1   
      
    %Datenpaket von Grösse "usart_frame_size" vom Mikrocontroller
    %empfangen
        
    ret_val = uint16(fread(s,usart_frame_size,'char'));  
        
    %Empfangenes Paket verarbeitet
    y=0;
    for z = 1:3:usart_frame_size                
            y=y+1;
            WaageBit(y)     = bitand(  16,ret_val(2+(z-1)))/16;           % Bit 4 Nullpos. Waage
            ReflBit(y)      = bitand(  32,ret_val(2+(z-1)))/32;           % Bit 5 Nullpos. Schweissbacken
            LichtBit(y)     = bitand(  64,ret_val(2+(z-1)))/64;           % Bit 6 Lichtschranke (Teilchen)
            TriggerBit(y)   = bitand( 128,ret_val(2+(z-1)))/128;          % Bit 7 Trigger vom MSP berechnet
            ADC(y) = ret_val(1+(z-1))+ bitand( 3,ret_val(2+(z-1)))*2^8;   % high Byte und Low Byte vertauscht wg. einfacherer Programmierung im µC
                                                                          % 10-Bit AD-Wandler, im Highbyte nur bit 1 & 2
    end
        
        ADCVals((1+((a-1)*170)):(a*170))        = ADC;  %Werte anhängen
        LichtVals((1+((a-1)*170)):(a*170))      = LichtBit;
        ReflVals((1+((a-1)*170)):(a*170))       = ReflBit;
        TriggerVals((1+((a-1)*170)):(a*170))    = TriggerBit;
        WaageVals((1+((a-1)*170)):(a*170))      = WaageBit;

    end
 fprintf(s,'%c','b');   % Beenden der Messung durch Senden von 'b'
 fclose(s); 
[sfileName,spathName]=uiputfile('.mat','Save Controller Data as',' ');
 %%
 %Plotten der Messwerte
 figure
 title('ADC-Value') 
 plot(ADCVals*(3.3/1024)), ylim([0 3.3]);
 hold all
 %plot(LichtVals*1024);
 %plot(TriggerVals*1024);
 %plot(ReflVals*1024);
 %legend('ADC','Licht','Trigger','Refl')
 
 h = figure;
 subplot(5,1,1)
 plot(ADCVals*(3.3/1024)),ylim([0 3.3]);
 grid
 legend('RF-Sensor analog') 
 
 subplot(5,1,2)
 plot(LichtVals) , ylim([0 1.5]);
 grid
 legend('Lichtschranke') 
 
 subplot(5,1,3)
 plot(TriggerVals), ylim([0 1.5]);
 grid
 legend('Sensor detection') 
 
 subplot(5,1,4)
 plot(ReflVals), ylim([0 1.5]);
 grid
 legend('Welding Bars')
 
 subplot(5,1,5)
 plot(WaageVals), ylim([0 1.5]);
 grid
 legend('Balance Signal')
 
 saveas(h,strrep(strcat(spathName,sfileName),'mat','fig'));

 save(strcat(spathName,sfileName),'SensorConfig','ADCVals','LichtVals','ReflVals','TriggerVals','WaageVals');
 