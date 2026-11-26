# IZTECH RoboLeague · Tasarla-Yap-Yarıştır hazırlık sayfası

Ödemiş BİLSEM ekibinin öğrenci sayfası: https://tkat35.github.io/iztech/

- `index.html` — tek sayfa (GitHub Pages, `main` dalı, kök klasör).
- Öğrenciler mBlock 5 ile blokla çalışır; sayfada her uygulamanın blok akışı vardır, metin kod gösterilmez.
- `kodlar/` — aynı uygulamaların Arduino Uno kodları. Sayfada yalnız "Evde → Wokwi kodları" düğmeleriyle kopyalanır.
  - `00` başlangıç taslağı · `01–12` temel uygulamalar · `13–14` P ve PD · `15–20` pist senaryoları
- `wokwi/diagram.json` — Wokwi'deki sanal robot devresi (pinler gerçek robotla aynı; motorlar LED, çizgi sensörü 4 anahtar).
- `araclar/kodlari_gom.py` — `kodlar/` içindeki bir kod değişince sayfadaki kopyasını günceller:
  `python3 araclar/kodlari_gom.py`

Pinler sayfadaki "Ortak bağlantı tablosu"na göredir.
