//
// Created: Fri May  3 13:53:49 2002
//      by: gensymbolfontmap.py
//    from: symbol.xml
//
// WARNING! All changes made in this file will be lost!

char symbolFontMap_in[] = "in";
char symbolFontMap_notin[] = "notin";
char symbolFontMap_partial[] = "partial";
char symbolFontMap_exists[] = "exists";
char symbolFontMap_forall[] = "forall";
char symbolFontMap_nabla[] = "nabla";
char symbolFontMap_oslash[] = "oslash";
char symbolFontMap_oplus[] = "oplus";
char symbolFontMap_otimes[] = "otimes";
char symbolFontMap_prod[] = "prod";
char symbolFontMap_ni[] = "ni";
char symbolFontMap_rightarrow[] = "rightarrow";
char symbolFontMap_downarrow[] = "downarrow";
char symbolFontMap_leftarrow[] = "leftarrow";
char symbolFontMap_uparrow[] = "uparrow";
char symbolFontMap_leftrightarrow[] = "leftrightarrow";
char symbolFontMap_wedge[] = "wedge";
char symbolFontMap_angle[] = "angle";
char symbolFontMap_degree[] = "degree";
char symbolFontMap_pm[] = "pm";
char symbolFontMap_vee[] = "vee";
char symbolFontMap_cap[] = "cap";
char symbolFontMap_prime[] = "prime";
char symbolFontMap_cup[] = "cup";
char symbolFontMap_sharp[] = "sharp";
char symbolFontMap_times[] = "times";
char symbolFontMap_xi[] = "xi";
char symbolFontMap_nu[] = "nu";
char symbolFontMap_lambda[] = "lambda";
char symbolFontMap_kappa[] = "kappa";
char symbolFontMap_langle[] = "langle";
char symbolFontMap_rangle[] = "rangle";
char symbolFontMap_int[] = "int";
char symbolFontMap_iota[] = "iota";
char symbolFontMap_theta[] = "theta";
char symbolFontMap_eta[] = "eta";
char symbolFontMap_zeta[] = "zeta";
char symbolFontMap_varepsilon[] = "varepsilon";
char symbolFontMap_delta[] = "delta";
char symbolFontMap_gamma[] = "gamma";
char symbolFontMap_beta[] = "beta";
char symbolFontMap_alpha[] = "alpha";
char symbolFontMap_Xi[] = "Xi";
char symbolFontMap_Lambda[] = "Lambda";
char symbolFontMap_Theta[] = "Theta";
char symbolFontMap_Delta[] = "Delta";
char symbolFontMap_div[] = "div";
char symbolFontMap_Gamma[] = "Gamma";
char symbolFontMap_vartheta[] = "vartheta";
char symbolFontMap_Upsilon[] = "Upsilon";
char symbolFontMap_varpi[] = "varpi";
char symbolFontMap_subset[] = "subset";
char symbolFontMap_supset[] = "supset";
char symbolFontMap_subseteq[] = "subseteq";
char symbolFontMap_supseteq[] = "supseteq";
char symbolFontMap_nsubset[] = "nsubset";
char symbolFontMap_heartsuit[] = "heartsuit";
char symbolFontMap_surd[] = "surd";
char symbolFontMap_infty[] = "infty";
char symbolFontMap_propto[] = "propto";
char symbolFontMap_neg[] = "neg";
char symbolFontMap_Re[] = "Re";
char symbolFontMap_o[] = "o";
char symbolFontMap_Im[] = "Im";
char symbolFontMap_mu[] = "mu";
char symbolFontMap_wp[] = "wp";
char symbolFontMap_diamondsuit[] = "diamondsuit";
char symbolFontMap_spadesuit[] = "spadesuit";
char symbolFontMap_clubsuit[] = "clubsuit";
char symbolFontMap_Leftrightarrow[] = "Leftrightarrow";
char symbolFontMap_minus[] = "minus";
char symbolFontMap_Uparrow[] = "Uparrow";
char symbolFontMap_Leftarrow[] = "Leftarrow";
char symbolFontMap_Downarrow[] = "Downarrow";
char symbolFontMap_Rightarrow[] = "Rightarrow";
char symbolFontMap_sum[] = "sum";
char symbolFontMap_ast[] = "ast";
char symbolFontMap_ldots[] = "ldots";
char symbolFontMap_rbrace[] = "rbrace";
char symbolFontMap_bullet[] = "bullet";
char symbolFontMap_sim[] = "sim";
char symbolFontMap_Phi[] = "Phi";
char symbolFontMap_Sigma[] = "Sigma";
char symbolFontMap_Pi[] = "Pi";
char symbolFontMap_Psi[] = "Psi";
char symbolFontMap_Omega[] = "Omega";
char symbolFontMap_aleph[] = "aleph";
char symbolFontMap_therefore[] = "therefore";
char symbolFontMap_mid[] = "mid";
char symbolFontMap_nsime[] = "nsime";
char symbolFontMap_cong[] = "cong";
char symbolFontMap_cdot[] = "cdot";
char symbolFontMap_approx[] = "approx";
char symbolFontMap_pi[] = "pi";
char symbolFontMap_rho[] = "rho";
char symbolFontMap_sigma[] = "sigma";
char symbolFontMap_tau[] = "tau";
char symbolFontMap_upsilon[] = "upsilon";
char symbolFontMap_phi[] = "phi";
char symbolFontMap_chi[] = "chi";
char symbolFontMap_psi[] = "psi";
char symbolFontMap_omega[] = "omega";
char symbolFontMap_Diamond[] = "Diamond";
char symbolFontMap_greater[] = "greater";
char symbolFontMap_less[] = "less";
char symbolFontMap_colon[] = "colon";
char symbolFontMap_neq[] = "neq";
char symbolFontMap_equiv[] = "equiv";
char symbolFontMap_leq[] = "leq";
char symbolFontMap_geq[] = "geq";
char symbolFontMap_lbrace[] = "lbrace";
char symbolFontMap_carriagereturn[] = "carriagereturn";
char symbolFontMap_lbrack[] = "lbrack";
char symbolFontMap_rbrack[] = "rbrack";
char symbolFontMap_varphi[] = "varphi";

struct { int unicode; uchar pos; CharClass cl; char* latexName; } symbolFontMap[] = {
    { 0x2208, 206, RELATION, symbolFontMap_in },
    { 0x2209, 207, RELATION, symbolFontMap_notin },
    { 0x2202, 182, ORDINARY, symbolFontMap_partial },
    { 0x2203, 36, ORDINARY, symbolFontMap_exists },
    { 0x2200, 34, ORDINARY, symbolFontMap_forall },
    { 0x2207, 209, ORDINARY, symbolFontMap_nabla },
    { 0x2205, 198, BINOP, symbolFontMap_oslash },
    { 0x2295, 197, BINOP, symbolFontMap_oplus },
    { 0x2297, 196, BINOP, symbolFontMap_otimes },
    { 0x220F, 213, ORDINARY, symbolFontMap_prod },
    { 0x220D, 39, RELATION, symbolFontMap_ni },
    { 0x2192, 174, RELATION, symbolFontMap_rightarrow },
    { 0x2193, 175, RELATION, symbolFontMap_downarrow },
    { 0x2190, 172, RELATION, symbolFontMap_leftarrow },
    { 0x2191, 173, RELATION, symbolFontMap_uparrow },
    { 0x2194, 171, RELATION, symbolFontMap_leftrightarrow },
    { 0x2227, 217, BINOP, symbolFontMap_wedge },
    { 0x2220, 208, ORDINARY, symbolFontMap_angle },
    { 0x00B0, 176, ORDINARY, symbolFontMap_degree },
    { 0x00B1, 177, BINOP, symbolFontMap_pm },
    { 0x002B, 43, BINOP, 0 },
    { 0x002C, 44, PUNCTUATION, 0 },
    { 0x2228, 218, BINOP, symbolFontMap_vee },
    { 0x2229, 199, BINOP, symbolFontMap_cap },
    { 0x002F, 47, ORDINARY, 0 },
    { 0x2033, 178, ORDINARY, 0 },
    { 0x2032, 162, ORDINARY, symbolFontMap_prime },
    { 0x0028, 40, ORDINARY, 0 },
    { 0x0029, 41, ORDINARY, 0 },
    { 0x222A, 200, BINOP, symbolFontMap_cup },
    { 0x0020, 32, ORDINARY, 0 },
    { 0x0021, 33, ORDINARY, 0 },
    { 0x0023, 35, ORDINARY, symbolFontMap_sharp },
    { 0x0025, 37, ORDINARY, 0 },
    { 0x0026, 38, ORDINARY, 0 },
    { 0x00D7, 180, BINOP, symbolFontMap_times },
    { 0x2122, 212, ORDINARY, 0 },
    { 0x03BE, 120, ORDINARY, symbolFontMap_xi },
    { 0x03BD, 110, ORDINARY, symbolFontMap_nu },
    { 0x03BB, 108, ORDINARY, symbolFontMap_lambda },
    { 0x03BA, 107, ORDINARY, symbolFontMap_kappa },
    { 0x2329, 225, ORDINARY, symbolFontMap_langle },
    { 0x2321, 245, ORDINARY, 0 },
    { 0x2320, 243, ORDINARY, 0 },
    { 0xF8F6, 246, ORDINARY, 0 },
    { 0xF8F7, 247, ORDINARY, 0 },
    { 0xF8F4, 239, ORDINARY, 0 },
    { 0xF8F5, 244, ORDINARY, 0 },
    { 0xF8F2, 237, ORDINARY, 0 },
    { 0xF8F3, 238, ORDINARY, 0 },
    { 0xF8F0, 235, ORDINARY, 0 },
    { 0xF8F1, 236, ORDINARY, 0 },
    { 0xF8F8, 248, ORDINARY, 0 },
    { 0xF8F9, 249, ORDINARY, 0 },
    { 0xF8FD, 253, ORDINARY, 0 },
    { 0xF8FE, 254, ORDINARY, 0 },
    { 0xF8FB, 251, ORDINARY, 0 },
    { 0xF8FC, 252, ORDINARY, 0 },
    { 0xF8FA, 250, ORDINARY, 0 },
    { 0x232A, 241, ORDINARY, symbolFontMap_rangle },
    { 0x222B, 242, ORDINARY, symbolFontMap_int },
    { 0x03B9, 105, ORDINARY, symbolFontMap_iota },
    { 0x03B8, 113, ORDINARY, symbolFontMap_theta },
    { 0x03B7, 104, ORDINARY, symbolFontMap_eta },
    { 0x03B6, 122, ORDINARY, symbolFontMap_zeta },
    { 0x03B5, 101, ORDINARY, symbolFontMap_varepsilon },
    { 0x03B4, 100, ORDINARY, symbolFontMap_delta },
    { 0x03B3, 103, ORDINARY, symbolFontMap_gamma },
    { 0x03B2, 98, ORDINARY, symbolFontMap_beta },
    { 0x03B1, 97, ORDINARY, symbolFontMap_alpha },
    { 0x039F, 79, ORDINARY, 0 },
    { 0x039D, 78, ORDINARY, 0 },
    { 0x039E, 88, ORDINARY, symbolFontMap_Xi },
    { 0x039B, 76, ORDINARY, symbolFontMap_Lambda },
    { 0x039C, 77, ORDINARY, 0 },
    { 0x039A, 75, ORDINARY, 0 },
    { 0x0398, 81, ORDINARY, symbolFontMap_Theta },
    { 0x0399, 73, ORDINARY, 0 },
    { 0x0396, 90, ORDINARY, 0 },
    { 0x0397, 72, ORDINARY, 0 },
    { 0x0394, 68, ORDINARY, symbolFontMap_Delta },
    { 0x0395, 69, ORDINARY, 0 },
    { 0x00F7, 184, BINOP, symbolFontMap_div },
    { 0x0393, 71, ORDINARY, symbolFontMap_Gamma },
    { 0x0391, 65, ORDINARY, 0 },
    { 0x03D1, 74, ORDINARY, symbolFontMap_vartheta },
    { 0x03D2, 161, ORDINARY, symbolFontMap_Upsilon },
    { 0x03D6, 118, ORDINARY, symbolFontMap_varpi },
    { 0x2282, 204, RELATION, symbolFontMap_subset },
    { 0x2283, 201, RELATION, symbolFontMap_supset },
    { 0x2286, 205, RELATION, symbolFontMap_subseteq },
    { 0x2287, 202, RELATION, symbolFontMap_supseteq },
    { 0x2284, 203, RELATION, symbolFontMap_nsubset },
    { 0x2665, 169, ORDINARY, symbolFontMap_heartsuit },
    { 0x221A, 214, ORDINARY, symbolFontMap_surd },
    { 0x221E, 165, ORDINARY, symbolFontMap_infty },
    { 0x221D, 181, RELATION, symbolFontMap_propto },
    { 0x00AE, 210, ORDINARY, 0 },
    { 0x0192, 166, ORDINARY, 0 },
    { 0x00AC, 216, ORDINARY, symbolFontMap_neg },
    { 0x211C, 194, ORDINARY, symbolFontMap_Re },
    { 0x03BF, 111, ORDINARY, symbolFontMap_o },
    { 0x2111, 193, ORDINARY, symbolFontMap_Im },
    { 0x03BC, 109, ORDINARY, symbolFontMap_mu },
    { 0x2118, 195, ORDINARY, symbolFontMap_wp },
    { 0x00A9, 211, ORDINARY, 0 },
    { 0x2666, 168, ORDINARY, symbolFontMap_diamondsuit },
    { 0x2660, 170, ORDINARY, symbolFontMap_spadesuit },
    { 0x2663, 167, ORDINARY, symbolFontMap_clubsuit },
    { 0x21D4, 219, RELATION, symbolFontMap_Leftrightarrow },
    { 0x2212, 45, BINOP, symbolFontMap_minus },
    { 0x21D1, 221, RELATION, symbolFontMap_Uparrow },
    { 0x21D0, 220, RELATION, symbolFontMap_Leftarrow },
    { 0x21D3, 223, RELATION, symbolFontMap_Downarrow },
    { 0x21D2, 222, RELATION, symbolFontMap_Rightarrow },
    { 0x2211, 229, ORDINARY, symbolFontMap_sum },
    { 0x2217, 42, BINOP, symbolFontMap_ast },
    { 0x2026, 188, ORDINARY, symbolFontMap_ldots },
    { 0x007D, 125, ORDINARY, symbolFontMap_rbrace },
    { 0x2022, 183, BINOP, symbolFontMap_bullet },
    { 0x223C, 126, RELATION, symbolFontMap_sim },
    { 0x03A6, 70, ORDINARY, symbolFontMap_Phi },
    { 0x03A7, 67, ORDINARY, 0 },
    { 0x03A4, 84, ORDINARY, 0 },
    { 0x03A5, 85, ORDINARY, 0 },
    { 0x03A3, 83, ORDINARY, symbolFontMap_Sigma },
    { 0x03A0, 80, ORDINARY, symbolFontMap_Pi },
    { 0x03A1, 82, ORDINARY, 0 },
    { 0x22A5, 94, ORDINARY, 0 },
    { 0x03A8, 89, ORDINARY, symbolFontMap_Psi },
    { 0x03A9, 87, ORDINARY, symbolFontMap_Omega },
    { 0x2135, 192, ORDINARY, symbolFontMap_aleph },
    { 0x2234, 92, BINOP, symbolFontMap_therefore },
    { 0x007C, 124, ORDINARY, symbolFontMap_mid },
    { 0x0037, 55, ORDINARY, 0 },
    { 0x0036, 54, ORDINARY, 0 },
    { 0x0035, 53, ORDINARY, 0 },
    { 0x0034, 52, ORDINARY, 0 },
    { 0x0033, 51, ORDINARY, 0 },
    { 0x0032, 50, ORDINARY, 0 },
    { 0x0031, 49, ORDINARY, 0 },
    { 0x0030, 48, ORDINARY, 0 },
    { 0x0039, 57, ORDINARY, 0 },
    { 0x0038, 56, ORDINARY, 0 },
    { 0x2244, 164, ORDINARY, symbolFontMap_nsime },
    { 0x2245, 64, RELATION, symbolFontMap_cong },
    { 0x22C5, 215, BINOP, symbolFontMap_cdot },
    { 0x0392, 66, ORDINARY, 0 },
    { 0x2248, 187, RELATION, symbolFontMap_approx },
    { 0x03C0, 112, ORDINARY, symbolFontMap_pi },
    { 0x03C1, 114, ORDINARY, symbolFontMap_rho },
    { 0x03C2, 86, ORDINARY, 0 },
    { 0x03C3, 115, ORDINARY, symbolFontMap_sigma },
    { 0x03C4, 116, ORDINARY, symbolFontMap_tau },
    { 0x03C5, 117, ORDINARY, symbolFontMap_upsilon },
    { 0x03C6, 102, ORDINARY, symbolFontMap_phi },
    { 0x03C7, 99, ORDINARY, symbolFontMap_chi },
    { 0x03C8, 121, ORDINARY, symbolFontMap_psi },
    { 0x03C9, 119, ORDINARY, symbolFontMap_omega },
    { 0x26C4, 224, ORDINARY, symbolFontMap_Diamond },
    { 0x003F, 63, ORDINARY, 0 },
    { 0x003E, 62, RELATION, symbolFontMap_greater },
    { 0x003D, 61, RELATION, 0 },
    { 0x003C, 60, RELATION, symbolFontMap_less },
    { 0x003B, 59, PUNCTUATION, 0 },
    { 0x003A, 58, PUNCTUATION, symbolFontMap_colon },
    { 0xF8EF, 234, ORDINARY, 0 },
    { 0xF8EE, 233, ORDINARY, 0 },
    { 0xF8ED, 232, ORDINARY, 0 },
    { 0xF8EC, 231, ORDINARY, 0 },
    { 0xF8EB, 230, ORDINARY, 0 },
    { 0xF8EA, 228, ORDINARY, 0 },
    { 0x2260, 185, RELATION, symbolFontMap_neq },
    { 0x2261, 186, RELATION, symbolFontMap_equiv },
    { 0x2264, 163, RELATION, symbolFontMap_leq },
    { 0x2265, 179, RELATION, symbolFontMap_geq },
    { 0x002E, 46, PUNCTUATION, 0 },
    { 0x007B, 123, ORDINARY, symbolFontMap_lbrace },
    { 0x21B5, 191, ORDINARY, symbolFontMap_carriagereturn },
    { 0x005B, 91, ORDINARY, symbolFontMap_lbrack },
    { 0x005D, 93, ORDINARY, symbolFontMap_rbrack },
    { 0x005F, 95, ORDINARY, 0 },
    { 0x03D5, 106, ORDINARY, symbolFontMap_varphi },
    { 0xF8E9, 227, ORDINARY, 0 },
    { 0xF8E8, 226, ORDINARY, 0 },
    { 0xF8E7, 190, ORDINARY, 0 },
    { 0xF8E6, 189, RELATION, 0 },
    { 0xF8E5, 96, ORDINARY, 0 },
    { 0, 0, ORDINARY, 0 }
};

