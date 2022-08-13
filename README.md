# wch-ch569-serdes reverse-engineering
Aim of this project:
* To reverse the WCH CH569 SerDes to interface it with FPGA...
  * See documents:
    * [WCH_CH569_SerDes_ReverseEngineering_BVERNOUX_31July2022.pdf](WCH_CH569_SerDes_ReverseEngineering_BVERNOUX_31July2022.pdf)
    * [WCH_CH569_SerDes_ReverseEngineering_BVERNOUX_31July2022.odt](WCH_CH569_SerDes_ReverseEngineering_BVERNOUX_31July2022.odt)
  * See also repositories:
    *  [CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex](CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex)
    *  [CH569_SerDes_180Mbps](CH569_SerDes_180Mbps)
* To reverse/rewrite all interesting blob/code to clean C code to have fully open source driver for WCH CH569 SerDes peripheral
  * Done in https://github.com/hydrausb3/hydrausb3_fw/tree/main/HydraUSB3_DualBoard_SerDes
  * See drv open source code API / code
    * https://github.com/hydrausb3/wch-ch56x-bsp/tree/main/drv/CH56x_serdes.c
    * https://github.com/hydrausb3/wch-ch56x-bsp/tree/main/drv/CH56x_serdes.h
