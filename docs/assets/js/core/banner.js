// {{- if site.Params.banner }}
// Override of Hextra v0.12.2's assets/js/core/banner.js: the theme version
// assumes .hextra-banner exists in the DOM and throws when it doesn't, which
// aborts the whole concatenated main.js bundle (including the mobile menu
// handler). Guard it so builds without a banner keep working.
(function () {
  const banner = document.querySelector(".hextra-banner");
  if (!banner) return;
  document.documentElement.style.setProperty("--hextra-banner-height", banner.clientHeight + "px");

  const closeBtn = banner.querySelector(".hextra-banner-close-button");

  closeBtn.addEventListener("click", () => {
    document.documentElement.classList.add("hextra-banner-hidden");
    document.documentElement.style.setProperty("--hextra-banner-height", "0px");

    localStorage.setItem('{{ site.Params.banner.key | default `banner-closed` }}', "0");
  });
})();
// {{- end -}}
