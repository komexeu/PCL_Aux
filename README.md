# PCL_Aux  
***version:**  tmp_version 0.5a*
***平台 :** Window 10 x64*

## 簡介 :
> 尚未完成之EXE檔，僅供臨時分割使用。

## 安裝說明:
> 將Aux_release1.7z解壓縮

> 將下方IntelRealsense.dll / realsense2.dll / PCLAuxilary.exe覆蓋進Aux_release1.7z解壓縮後的檔案。

> 點選PCLAuxilary.exe即可直接使用。

## 使用方法:
### Data Import(點雲輸入):
* 可輸入.pcd與.csv
> 使用限制 :
> - pcd格式為PointXYZRGB(x,y,z,rgb)
> - csv格式為(x,y,z,r,g,b)
> - 點雲超過400,000點時載入速度較慢

### Data Export(點雲輸出):
* 輸出.pcd
* 點選一個圖層，自動抓取底下已分割完成的所有子類圖層資訊
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/version_05a_1.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/version_05a_2.png)
> 使用限制 :
> - 僅輸出.pcd(x,y,z,rgb,ID)

### realsense point pick(取像模式):
> 使用限制 :
>- 暫時鎖定，無法使用
 
### Slider Segmentate(滑條分割模式):
 * 拉動滑條進行快速分割預覽 ![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/version_05a_3.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/tmp_version_05a_4.png)
* 點選Slider Segmentate確定分割。
> 使用限制 :
>- 因效能問題暫時將可處理資料點設定在100,000內。
>- 大於100,000將無法進行分割，請使用手動分割。

### User Segmentate(已選點手動分割):
* 對手動分割工具(X框選/B筆刷)進行標記。
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/tmp_version_05a_5.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/tmp_version_05a_6.png)
* 點選User Segmentate確定分割。

### smooth(平滑化):
* 對不平滑點雲進行平滑化
> 使用限制 :
>- 資料點太大或電腦效能較不佳處理時序較久，有可能造成崩潰。
 
 ## 工具快捷鍵
|快捷鍵|功能| 備註 | 
|-----|----|-----|
| X |手動標記:框選(建議使用選項) |按下"X"來啟動/取消使用工具|
|B|手動標記:筆刷(資料點太多時效能較差)|按下"B"來啟動/取消使用工具。|
|A|視角轉換|六方位視角變換|
|Ctrl|手動標記中使用，加選|
|Alt|手動標記中使用，減選|
|N|筆刷縮小|
|M|筆刷放大|太大時效能較差，有可能當機

## 使用範例:
> "Data Import"匯入點雲:

![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_1.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_2.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_3.png)

> 點雲數量為921599，手動將資料量降低，步驟:

 1. 按下"x"鍵開啟框選標記功能。
 2. 標記完畢後按下左側"User Segmentate"。
 
 ![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_4.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_5.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_6.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_7.png)

> 快速群集分割，步驟:

 1. 使用底下滑條來做快速基本分割，以不同顏色來分割表示結果預覽。
 2. 確定範圍後按下"Slider Segmentate"來做分割。

![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_8.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_9.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_10.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_11.png)
> 使用垃圾桶刪除不要的圖層。

![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_13.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_14.png)

> 按下"Data Export"輸出資料(紅框內為選取不同圖層後，對應輸出的圖層)。

 - 點選20200619154445.pcd對應輸出的圖層數為2(ID 為 0,1)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_15.png)
 - 點選point_nd_decrease_0.pcd對應輸出的圖層數為5(ID 為 0,1,2,3,4)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_16.png)
> 輸出資料內容
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_18.png)
![enter image description here](https://github.com/komexeu/Pic_Wareh/blob/master/Usage%20example_17.png)
