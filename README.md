# object-tracking-with-edge-extraction

Mühendislik tasarımı ve bitirme projemizin bir kısmı olan gri görüntüde nesne takibi için tasarladığım algoritma.
Görüntüde seçilen alanın canny algoritması yardımı ile kenarları çıkarılıyor ve bir sonraki framede, seçilen alanın belirlenen bir pixel aralığı etrafında yine canny algoritması ile kenarları çıkarılıyor.
Daha sonra aranan nesne ile taranan alan arasındaki binary görüntüler kıyaslanarak en fazla uyuşmanın olduğu yer belirlenen yeni alan(nesnenin yeni konumu) oluyor. </br>

Algoritma calışsada çok istikrarlı bir takip sağlanamıyor bu yüzden kendi algoritmamız yerine bilinen bir algoritmaya geçtik bu algortima ile nesne takibi videosunu yakında paylaşıcam.
