### Example capture & descamble of CH569 SerDes at 180Mbps

* Rigol MSO5000 Series bin capture file: `CH569_SerDes_180Mbps.bin`
* Pre-configured glscopeclient session file: `CH569_SerDes_180Mbps.bin.scopesession`
* glscopeclient exported "8b10bIBM_Hex" CSV data: `CH569_SerDes_180Mbps.csv`


* CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex descrambled results (using `CH569_SerDes_180Mbps.csv` as input): `SerDes_180Mbps.csv_res.txt`
```
CH569 SerDes data capture extractor(Descramble/check CRC32) v1.0 by B.VERNOUX 08 Jun 2022

SerDes SOF data(HEX) packet1 at 0.00002350s/23.50us:
0FFFFFFF (SerDes HDR at 0.00002372s/23.72us)
5A5A5A5A 15344752 (SerDes CRC32 OK(Nb32bits=2) at 0.00002439s/24.39us)
```
