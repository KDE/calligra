//
// Created: Fri Jun  8 19:00:18 2001
//      by: gensymbolfontmap.py
//    from: symbol.xml
//
// WARNING! All changes made in this file will be lost!

char symbolFontMap_varpi[] = "\\varpi";
char symbolFontMap_phi[] = "\\phi";
char symbolFontMap_Upsilon[] = "\\Upsilon";
char symbolFontMap_vartheta[] = "\\vartheta";
char symbolFontMap_cup[] = "\\cup";
char symbolFontMap_Gamma[] = "\\Gamma";
char symbolFontMap_Delta[] = "\\Delta";
char symbolFontMap_therefore[] = "\\therefore";
char symbolFontMap_Theta[] = "\\Theta";
char symbolFontMap_div[] = "\\div";
char symbolFontMap_neq[] = "\\neq";
char symbolFontMap_spadesuit[] = "\\spadesuit";
char symbolFontMap_Omega[] = "\\Omega";
char symbolFontMap_Psi[] = "\\Psi";
char symbolFontMap_Pi[] = "\\Pi";
char symbolFontMap_Sigma[] = "\\Sigma";
char symbolFontMap_times[] = "\\times";
char symbolFontMap_Lambda[] = "\\Lambda";
char symbolFontMap_Xi[] = "\\Xi";
char symbolFontMap_geq[] = "\\geq";
char symbolFontMap_leq[] = "\\leq";
char symbolFontMap_equiv[] = "\\equiv";
char symbolFontMap_sim[] = "\\sim";
char symbolFontMap_bullet[] = "\\bullet";
char symbolFontMap_cap[] = "\\cap";
char symbolFontMap_vee[] = "\\vee";
char symbolFontMap_pm[] = "\\pm";
char symbolFontMap_ldots[] = "\\ldots";
char symbolFontMap_angle[] = "\\angle";
char symbolFontMap_wedge[] = "\\wedge";
char symbolFontMap_leftrightarrow[] = "\\leftrightarrow";
char symbolFontMap_downarrow[] = "\\downarrow";
char symbolFontMap_rightarrow[] = "\\rightarrow";
char symbolFontMap_alpha[] = "\\alpha";
char symbolFontMap_beta[] = "\\beta";
char symbolFontMap_gamma[] = "\\gamma";
char symbolFontMap_delta[] = "\\delta";
char symbolFontMap_varepsilon[] = "\\varepsilon";
char symbolFontMap_varsigma[] = "\\varsigma";
char symbolFontMap_eta[] = "\\eta";
char symbolFontMap_theta[] = "\\theta";
char symbolFontMap_iota[] = "\\iota";
char symbolFontMap_Rightarrow[] = "\\Rightarrow";
char symbolFontMap_Downarrow[] = "\\Downarrow";
char symbolFontMap_Leftarrow[] = "\\Leftarrow";
char symbolFontMap_Uparrow[] = "\\Uparrow";
char symbolFontMap_uparrow[] = "\\uparrow";
char symbolFontMap_leftarrow[] = "\\leftarrow";
char symbolFontMap_Leftrightarrow[] = "\\Leftrightarrow";
char symbolFontMap_sum[] = "\\sum";
char symbolFontMap_approx[] = "\\approx";
char symbolFontMap_cdot[] = "\\cdot";
char symbolFontMap_clubsuit[] = "\\clubsuit";
char symbolFontMap_prod[] = "\\prod";
char symbolFontMap_diamondsuit[] = "\\diamondsuit";
char symbolFontMap_heartsuit[] = "\\heartsuit";
char symbolFontMap_ni[] = "\\ni";
char symbolFontMap_cong[] = "\\cong";
char symbolFontMap_otimes[] = "\\otimes";
char symbolFontMap_oplus[] = "\\oplus";
char symbolFontMap_omega[] = "\\omega";
char symbolFontMap_psi[] = "\\psi";
char symbolFontMap_chi[] = "\\chi";
char symbolFontMap_varphi[] = "\\varphi";
char symbolFontMap_upsilon[] = "\\upsilon";
char symbolFontMap_tau[] = "\\tau";
char symbolFontMap_sigma[] = "\\sigma";
char symbolFontMap_rho[] = "\\rho";
char symbolFontMap_pi[] = "\\pi";
char symbolFontMap_oslash[] = "\\oslash";
char symbolFontMap_neg[] = "\\neg";
char symbolFontMap_nable[] = "\\nable";
char symbolFontMap_forall[] = "\\forall";
char symbolFontMap_exists[] = "\\exists";
char symbolFontMap_partial[] = "\\partial";
char symbolFontMap_in[] = "\\in";
char symbolFontMap_Im[] = "\\Im";
char symbolFontMap_propto[] = "\\propto";
char symbolFontMap_infty[] = "\\infty";
char symbolFontMap_surd[] = "\\surd";
char symbolFontMap_Re[] = "\\Re";
char symbolFontMap_lambda[] = "\\lambda";
char symbolFontMap_mu[] = "\\mu";
char symbolFontMap_nu[] = "\\nu";
char symbolFontMap_xi[] = "\\xi";
char symbolFontMap_o[] = "o";
char symbolFontMap_supseteq[] = "\\supseteq";
char symbolFontMap_subseteq[] = "\\subseteq";
char symbolFontMap_kappa[] = "\\kappa";
char symbolFontMap_supset[] = "\\supset";
char symbolFontMap_subset[] = "\\subset";

struct { int unicode; uchar pos; CharClass cl; char* latexName; } symbolFontMap[] = {
    { 0x03D6, 118, ORDINARY, symbolFontMap_varpi },
    { 0x03D5, 102, ORDINARY, symbolFontMap_phi },
    { 0x03D2, 161, ORDINARY, symbolFontMap_Upsilon },
    { 0x03D1, 74, ORDINARY, symbolFontMap_vartheta },
    { 0x0399, 73, ORDINARY, 0 },
    { 0x222A, 200, BINOP, symbolFontMap_cup },
    { 0x0391, 65, ORDINARY, 0 },
    { 0x21B5, 191, ORDINARY, 0 },
    { 0x0393, 71, ORDINARY, symbolFontMap_Gamma },
    { 0x0392, 66, ORDINARY, 0 },
    { 0x0395, 69, ORDINARY, 0 },
    { 0x0394, 68, ORDINARY, symbolFontMap_Delta },
    { 0x0397, 72, ORDINARY, 0 },
    { 0x2234, 92, BINOP, symbolFontMap_therefore },
    { 0x0398, 81, ORDINARY, symbolFontMap_Theta },
    { 0x00F7, 184, BINOP, symbolFontMap_div },
    { 0x2260, 185, RELATION, symbolFontMap_neq },
    { 0x2660, 170, ORDINARY, symbolFontMap_spadesuit },
    { 0x03A9, 87, ORDINARY, symbolFontMap_Omega },
    { 0x03A8, 89, ORDINARY, symbolFontMap_Psi },
    { 0x03A1, 82, ORDINARY, 0 },
    { 0x03A0, 80, ORDINARY, symbolFontMap_Pi },
    { 0x03A3, 83, ORDINARY, symbolFontMap_Sigma },
    { 0x03A5, 85, ORDINARY, 0 },
    { 0x03A4, 84, ORDINARY, 0 },
    { 0x03A7, 67, ORDINARY, 0 },
    { 0x03A6, 70, ORDINARY, 0 },
    { 0x039A, 75, ORDINARY, 0 },
    { 0x00D7, 180, BINOP, symbolFontMap_times },
    { 0x039C, 77, ORDINARY, 0 },
    { 0x039B, 76, ORDINARY, symbolFontMap_Lambda },
    { 0x039E, 88, ORDINARY, symbolFontMap_Xi },
    { 0x039D, 78, ORDINARY, 0 },
    { 0x039F, 79, ORDINARY, 0 },
    { 0x2265, 179, RELATION, symbolFontMap_geq },
    { 0x2264, 163, RELATION, symbolFontMap_leq },
    { 0x2261, 186, RELATION, symbolFontMap_equiv },
    { 0x223C, 126, RELATION, symbolFontMap_sim },
    { 0x00B7, 183, BINOP, symbolFontMap_bullet },
    { 0x2229, 199, BINOP, symbolFontMap_cap },
    { 0x2228, 218, BINOP, symbolFontMap_vee },
    { 0x00B1, 177, BINOP, symbolFontMap_pm },
    { 0x2026, 188, ORDINARY, symbolFontMap_ldots },
    { 0x00B4, 162, ORDINARY, 0 },
    { 0x2220, 208, ORDINARY, symbolFontMap_angle },
    { 0x2227, 217, BINOP, symbolFontMap_wedge },
    { 0x00B0, 176, ORDINARY, 0 },
    { 0x2194, 171, ORDINARY, symbolFontMap_leftrightarrow },
    { 0x2193, 175, ORDINARY, symbolFontMap_downarrow },
    { 0x2192, 174, ORDINARY, symbolFontMap_rightarrow },
    { 0x03B1, 97, ORDINARY, symbolFontMap_alpha },
    { 0x03B2, 98, ORDINARY, symbolFontMap_beta },
    { 0x03B3, 103, ORDINARY, symbolFontMap_gamma },
    { 0x03B4, 100, ORDINARY, symbolFontMap_delta },
    { 0x03B5, 101, ORDINARY, symbolFontMap_varepsilon },
    { 0x03B6, 122, ORDINARY, symbolFontMap_varsigma },
    { 0x03B7, 104, ORDINARY, symbolFontMap_eta },
    { 0x03B8, 113, ORDINARY, symbolFontMap_theta },
    { 0x03B9, 105, ORDINARY, symbolFontMap_iota },
    { 0x21D2, 222, ORDINARY, symbolFontMap_Rightarrow },
    { 0x21D3, 223, ORDINARY, symbolFontMap_Downarrow },
    { 0x21D0, 220, ORDINARY, symbolFontMap_Leftarrow },
    { 0x21D1, 221, ORDINARY, symbolFontMap_Uparrow },
    { 0x2191, 173, ORDINARY, symbolFontMap_uparrow },
    { 0x2190, 172, ORDINARY, symbolFontMap_leftarrow },
    { 0x21D4, 219, ORDINARY, symbolFontMap_Leftrightarrow },
    { 0x2211, 229, ORDINARY, symbolFontMap_sum },
    { 0x2248, 187, RELATION, symbolFontMap_approx },
    { 0x2219, 215, BINOP, symbolFontMap_cdot },
    { 0x2663, 167, ORDINARY, symbolFontMap_clubsuit },
    { 0x220F, 213, ORDINARY, symbolFontMap_prod },
    { 0x2666, 168, ORDINARY, symbolFontMap_diamondsuit },
    { 0x2665, 169, ORDINARY, symbolFontMap_heartsuit },
    { 0x220B, 39, RELATION, symbolFontMap_ni },
    { 0x224C, 64, RELATION, symbolFontMap_cong },
    { 0x03DB, 86, ORDINARY, 0 },
    { 0x2215, 164, ORDINARY, 0 },
    { 0x2297, 196, BINOP, symbolFontMap_otimes },
    { 0x2295, 197, BINOP, symbolFontMap_oplus },
    { 0x03C9, 119, ORDINARY, symbolFontMap_omega },
    { 0x03C8, 121, ORDINARY, symbolFontMap_psi },
    { 0x03C7, 99, ORDINARY, symbolFontMap_chi },
    { 0x03C6, 106, ORDINARY, symbolFontMap_varphi },
    { 0x03C5, 117, ORDINARY, symbolFontMap_upsilon },
    { 0x03C4, 116, ORDINARY, symbolFontMap_tau },
    { 0x03C3, 115, ORDINARY, symbolFontMap_sigma },
    { 0x03C1, 114, ORDINARY, symbolFontMap_rho },
    { 0x03C0, 112, ORDINARY, symbolFontMap_pi },
    { 0x2205, 198, BINOP, symbolFontMap_oslash },
    { 0x00AC, 216, ORDINARY, symbolFontMap_neg },
    { 0x2207, 209, ORDINARY, symbolFontMap_nable },
    { 0x2200, 34, ORDINARY, symbolFontMap_forall },
    { 0x2203, 36, ORDINARY, symbolFontMap_exists },
    { 0x2202, 182, ORDINARY, symbolFontMap_partial },
    { 0x2209, 207, RELATION, 0 },
    { 0x2208, 206, RELATION, symbolFontMap_in },
    { 0x1D574, 193, ORDINARY, symbolFontMap_Im },
    { 0x221D, 181, RELATION, symbolFontMap_propto },
    { 0x221E, 165, ORDINARY, symbolFontMap_infty },
    { 0x221A, 214, ORDINARY, symbolFontMap_surd },
    { 0x1D579, 194, ORDINARY, symbolFontMap_Re },
    { 0x03BB, 108, ORDINARY, symbolFontMap_lambda },
    { 0x03BC, 109, ORDINARY, symbolFontMap_mu },
    { 0x03BD, 110, ORDINARY, symbolFontMap_nu },
    { 0x03BE, 120, ORDINARY, symbolFontMap_xi },
    { 0x03BF, 111, ORDINARY, symbolFontMap_o },
    { 0x2284, 203, RELATION, 0 },
    { 0x2287, 202, RELATION, symbolFontMap_supseteq },
    { 0x2286, 205, RELATION, symbolFontMap_subseteq },
    { 0x03Ba, 107, ORDINARY, symbolFontMap_kappa },
    { 0x2283, 201, RELATION, symbolFontMap_supset },
    { 0x2282, 204, RELATION, symbolFontMap_subset },
    { 0, 0, ORDINARY, 0 }
};

