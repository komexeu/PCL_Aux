# PCL_Aux  
***version:**  V.1.0.0*

***平台 :** Window 10 x64*

***PCL版本:** V.1.11.1*


## V.1.0.0版本介面
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Viewer.png)

## 簡介 :
> 簡易版本，主要可進行點雲輸入/輸出/分割/平滑化。

## 安裝說明:
> 下載: https://github.com/komexeu/PCL_Aux/releases/tag/v.1.0.0_PCLv1.11.1_x64  下載msi安裝檔

> 下載上方exe檔，對舊EXE檔覆蓋

# 待更新功能&操作解:
### 1. 使用N/M鍵對筆刷放大縮小，螢幕上筆刷大小未縮放
      目前筆刷無法在停留狀態更新，移動滑鼠即可更新筆刷。
### 2. 可不可以點選按鈕啟用工具?
      目前暫時沒有啟用此功能，下次更新將會啟用。
### 3. 滑鼠使用完滑條或按鈕後鍵盤沒反應(已在新版EXE檔更新)
      目前裡外視窗按鍵未同步，可以在點完外部滑條或按鈕後，點一下內部視窗即可正常操作。
### 4. 點選圖層後當機了
      圖層間切換會重新載入點雲，點雲資料量大時，順暢度依電腦效能而定。
### 5. 按Smooth當機了
      Smooth處理時較吃效能，需要等候一段時間，或是將數值稍微調低，下次更新會在處理時間較長的運算上加上Progress Bar提示。
### 6. 拉動Slider Segmentate很卡
      為了顯示方便，目前方法較吃效能，可以直接更改左側數字，減少分割運算次數。      



# 使用方法:
### Data Import(點雲輸入):
* 可輸入.pcd與.csv
> 使用限制 :
> - pcd格式為PointXYZRGB(x,y,z,rgb)
> - csv格式為(x,y,z,r,g,b)
> - 點雲超過400,000點時載入速度較慢

### Data Export(點雲輸出):
* 輸出.pcd
* 點選一個圖層，自動抓取底下已分割完成的所有子類圖層資訊
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/version_05a_1.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/version_05a_2.png)
> 使用限制 :
> - 僅輸出.pcd(x,y,z,rgb,ID)

### realsense point pick(取像模式):
> 使用限制 :
>- 暫時鎖定，無法使用
 
### Slider Segmentate(滑條分割模式):
 * 拉動滑條進行快速分割預覽 ![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/version_05a_3.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/tmp_version_05a_4.png)
* 點選Slider Segmentate確定分割。
> 使用限制 :
>- 因效能問題暫時將可處理資料點設定在100,000內。
>- 大於100,000將無法進行分割，請使用手動分割。

### User Segmentate(已選點手動分割):
* 對手動分割工具(X框選/B筆刷)進行標記。
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/tmp_version_05a_5.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/tmp_version_05a_6.png)  
* 點選User Segmentate確定分割。

### smooth(平滑化):
* 對不平滑點雲進行平滑化
* 拉動Smooth Slider設定平滑參數(數值越大耗時越久)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/V100_smooth.png)
* 點選左方Smooth確定平滑化。

> 使用限制 :
>- 資料點太大或電腦效能較不佳處理時序較久，有可能造成崩潰。

### 筆刷調整:
* 在筆刷模式下拉動 Brush Slider 控制滑條大小。
* 在筆刷模式下使用"N"縮小筆刷，"M"放大筆刷。
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/V100_brush.png)
 
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

![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_1.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_2.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_3.png)

> 點雲數量為921599，手動將資料量降低，步驟:

 1. 按下"x"鍵開啟框選標記功能。
 2. 標記完畢後按下左側"User Segmentate"。
 
 ![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_4.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_5.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_6.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_7.png)

> 快速群集分割，步驟:

 1. 使用底下滑條來做快速基本分割，以不同顏色來分割表示結果預覽。
 2. 確定範圍後按下"Slider Segmentate"來做分割。

![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_8.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_9.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_10.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_11.png)
> 使用垃圾桶刪除不要的圖層。

![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_13.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_14.png)

> 按下"Data Export"輸出資料(紅框內為選取不同圖層後，對應輸出的圖層)。

 - 點選20200619154445.pcd對應輸出的圖層數為2(ID 為 0,1)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_15.png)
 - 點選point_nd_decrease_0.pcd對應輸出的圖層數為5(ID 為 0,1,2,3,4)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_16.png)
> 輸出資料內容

![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_18.png)
![enter image description here](https://github.com/komexeu/PCL_Aux/blob/Picture_readme/readme_image/Usage%20example_17.png)
