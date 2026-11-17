// Web sürümünün (index.html) ve yazdırma sayfasının (yazdir.html) ortak docsify ayarları.
// yazdir.html bu dosyadan önce window.NOT_YAZDIR = true tanımlar.
(function () {
  var YAZDIR = !!window.NOT_YAZDIR;
  var DEPO = 'https://github.com/erkanozhan/HighPerfCom';
  var SITE = 'https://erkanozhan.github.io/HighPerfCom/';

  // Bir not dosyasındaki göreli yolu (ör. "../images/x.png") deponun kök klasörüne göre yola çevirir.
  function kokYolu(dosya, hedef) {
    var parcalar = [];
    ((dosya || '').replace(/[^\/]*$/, '') + hedef).split('/').forEach(function (p) {
      if (p === '..') parcalar.pop(); else if (p && p !== '.') parcalar.push(p);
    });
    return parcalar.join('/') + (/\/$/.test(hedef) ? '/' : '');
  }

  function goreliMi(hedef) {
    return !/^(https?:|mailto:|data:|#|\/)/.test(hedef);
  }

  // Markdown bağlantı ve görsellerinin göreli hedeflerini sırayla işler.
  // Metin içinde bir düzey köşeli parantez olabilir: ![... input[type="email"] ...](yol)
  function goreliHedefler(md, islev) {
    return md.replace(/(!?)\[((?:[^\[\]\n]|\[[^\[\]\n]*\])*)\]\(([^)\s]+)\)/g, function (tum, resim, metin, hedef) {
      if (!goreliMi(hedef)) return tum;
      var yeni = islev(hedef, !!resim);
      return yeni == null ? tum : resim + '[' + metin + '](' + yeni + ')';
    });
  }

  // Uygulamaların .html dosyalarına verilen bağlantılar, sitede yayımlanan çalışır sayfayı açar.
  // Kod, stil ve diğer dosyalara (.c, LICENSE ...) verilen bağlantılar GitHub'daki dosyaya gider.
  function depoBaglantilari(md, dosya) {
    return goreliHedefler(md, function (hedef, resim) {
      if (resim || /\.md(#|$)/.test(hedef)) return null;
      var yol = kokYolu(dosya, hedef.replace(/#.*$/, ''));
      if (/\.html?$/.test(yol)) return SITE + yol;
      return DEPO + (/\/$/.test(yol) || yol === '' ? '/tree/main/' : '/blob/main/') + yol.replace(/\/$/, '');
    });
  }

  // Not içine HTML olarak yazılmış görsellerin (<img src="../images/...">) yolu. docsify bu yolları
  // sayfanın klasörüne göre değil sitenin köküne göre çözdüğü için köke göre yazılır.
  function htmlYollari(md, dosya) {
    var kod = false;
    return md.split('\n').map(function (satir) {
      if (/^\s*(```|~~~)/.test(satir)) { kod = !kod; return satir; }
      if (kod) return satir;
      return satir.replace(/(<img\b[^>]*\ssrc=")([^"]+)"/g, function (tum, bas, hedef) {
        return goreliMi(hedef) ? bas + kokYolu(dosya, hedef) + '"' : tum;
      });
    }).join('\n');
  }

  // GitHub, rakamla başlayan başlıklara "21-giris" biçiminde adres verir; docsify ise "_21-giris".
  function capalar(md) {
    return md.replace(/\]\(([^)\s#]*)#(\d)/g, ']($1#_$2');
  }

  // Bir not dosyasına sitede uygulanan tüm dönüşümler
  function notuDuzenle(md, dosya) {
    return capalar(htmlYollari(depoBaglantilari(md, dosya), dosya));
  }

  // Formül eklentisinden ÖNCE çalışması gerektiği için index.html bu eklentiyi, formül betiği
  // yüklendikten sonra listenin başına ekler. yazdir.html ise her bölüme notuDuzenle'yi kendisi uygular.
  function notDuzenleEklentisi(hook, vm) {
    hook.beforeEach(function (md) {
      // yazdir.html her bölümü kendi dosya yoluyla zaten düzenler; sayfa adresiyle ikinci kez düzenlenirse
      // site alt klasörde yayımlandığında (erkanozhan.github.io/<depo>/) görsel yolları bozulur.
      if (YAZDIR) return md;
      return notuDuzenle(md, (vm.route && vm.route.file) || '');
    });
    // Sayfa içi başlık bağlantısı hâlâ bir başlığa denk gelmiyorsa (docsify başlık adresini GitHub'dan farklı
    // üretebilir: kesme işareti, büyük harf gibi) harf ve rakamları bağlantıyla aynı olan başlık aranır.
    hook.doneEach(function () {
      var sade = function (s) { return s.toLowerCase().normalize('NFD').replace(/[^\p{L}\p{N}]/gu, '').replace(/ı/g, 'i'); };
      var basliklar = Array.prototype.slice.call(document.querySelectorAll('.markdown-section h1, .markdown-section h2, .markdown-section h3, .markdown-section h4'))
        .map(function (h) { return { id: h.id, metin: sade(h.textContent) }; });
      Array.prototype.forEach.call(document.querySelectorAll('.markdown-section a[href*="?id="]'), function (a) {
        var m = a.getAttribute('href').match(/^(#\/[^?]*)\?id=(.*)$/);
        if (!m || m[1] !== '#' + vm.route.path) return;
        var id = decodeURIComponent(m[2]);
        if (document.getElementById(id)) return;
        var bulunan = basliklar.filter(function (h) { return h.id && h.metin === sade(id); })[0];
        if (bulunan) a.setAttribute('href', m[1] + '?id=' + encodeURIComponent(bulunan.id));
      });
      var istenen = vm.route.query && vm.route.query.id;
      if (istenen && !document.getElementById(istenen)) {
        var hedef = basliklar.filter(function (h) { return h.id && h.metin === sade(istenen); })[0];
        if (hedef) setTimeout(function () { document.getElementById(hedef.id).scrollIntoView(); }, 0);
      }
    });
  }

  // GitHub uyarı kutularını (> [!CAUTION] vb.) docsify'da da renkli göster
  function uyariKutulari(hook) {
    var basliklar = { NOTE: 'Not', TIP: 'İpucu', IMPORTANT: 'Önemli', WARNING: 'Uyarı', CAUTION: 'Dikkat' };
    hook.afterEach(function (html) {
      return html.replace(/<blockquote>\s*<p>\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\]\s*(<br\s*\/?>)?/g, function (_, tur) {
        return '<blockquote class="uyari uyari-' + tur.toLowerCase() + '"><p><span class="uyari-baslik">' + basliklar[tur] + '</span>';
      });
    });
  }

  // yazdir.html de aynı dönüşümleri kullanır
  window.NOT_ARACLARI = {
    notuDuzenle: notuDuzenle,
    eklenti: notDuzenleEklentisi,
    // yazdırma sayfası kök klasörde çalışır: göreli görsel yolları köke göre yazılır
    kokeGore: function (md, dosya) {
      return goreliHedefler(md, function (hedef) { return kokYolu(dosya, hedef); });
    }
  };

  window.$docsify = {
    name: 'Yüksek Başarımlı Hesaplama',
    repo: DEPO,
    loadSidebar: !YAZDIR,
    alias: { '/.*/_sidebar.md': '/_sidebar.md' },
    subMaxLevel: 2,
    relativePath: true,
    auto2top: true,
    search: {
      paths: 'auto',
      depth: 3,
      placeholder: 'Notta ara',
      noData: 'Sonuç bulunamadı'
    },
    plugins: [uyariKutulari],
    markdown: {
      renderer: {
        code: function (code, lang) {
          // ```math blokları (GitHub'ın display formül biçimi) → KaTeX ile çiz
          if (lang === 'math' && window.katex) {
            return '<div class="math-blok">' +
              window.katex.renderToString(code, { displayMode: true, throwOnError: false }) +
              '</div>';
          }
          return this.origin.code.apply(this, arguments);
        }
      }
    }
  };
})();
