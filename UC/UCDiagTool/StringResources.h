#pragma once

#include <string>
#include <vector>
#include <map>

#define IDS_UPDATE_REQUIRES_REBOOT 0
#define IDS_ELEVATION_REQUIRED 1
#define DEFAULT_LANG_ID 1033

struct TranlationSet 
{
    int languageCode;
    std::vector<std::wstring> strings;
};

static TranlationSet _englishTranslationSet = {
    1033,
    {
        L"A Cisco software update requires a reboot to complete.",
        L"Administrative privileges are required to run correctly. Please run again from an Administrator account or with UAC enabled" 
    }
};

static TranlationSet _cs_cz = {
    1029,
    {
        L"Pro dokončení aktualizace softwaru Cisco je vyžadován restart.",
        L"Pro správné fungování jsou vyžadována oprávnění správce. Spusťte znovu z účtu správce nebo s povolenou kontrolou řízení přístupu uživatele (UAC)"
    }
};

static TranlationSet _de_de = {
    1031,
    {
        L"Ein Cisco Softwareupdate erfordert einen Neustart.",
        L"Für die korrekte Ausführung sind Administratorrechte erforderlich. Mit einem Administratorkonto oder mit aktivierter UAC (Benutzerkontensteuerung) erneut ausführen"
    }
};

static TranlationSet _es_es = {
    1034,
    {
        L"Una actualización de software de Cisco requiere un reinicio para completarse.",
        L"Se necesitan privilegios de administrador para ejecutarlo correctamente. Vuelva a ejecutar desde una cuenta de administrador o con UAC habilitado"
    }
};

static TranlationSet _fr_ca = {
    3084,
    {
        L"Une mise à jour logicielle Cisco nécessite un redémarrage pour se terminer.",
        L"Des privilèges d’administration sont nécessaires pour s’exécuter correctement. Veuillez l’exécuter à nouveau à partir d’un compte d’administrateur ou avec l’UAC activé"
    }
};

static TranlationSet _fr_fr = {
    1036,
    {
        L"Une mise à jour logicielle Cisco nécessite un redémarrage.",
        L"Des privilèges d'administrateur sont nécessaires pour une exécution correcte. Veuillez l'exécuter à nouveau à partir d'un compte d'administrateur ou avec le contrôle de compte d'utilisateur activé."
    }
};

static TranlationSet _hu_hu = {
    1038,
    {
        L"Egy Cisco szoftverfrissítés befejezéséhez újraindításra van szükség.",
        L"A helyes futtatáshoz rendszergazdai jogosultság szükséges. Kérjük, futtassa újra rendszergazdai fiókból vagy az UAC engedélyezésével."
    }
};

static TranlationSet _it_it = {
    1040,
    {
        L"Per completare l'aggiornamento del software Cisco è necessario riavviare il sistema.",
        L"Per un funzionamento corretto sono necessari privilegi di amministratore. Ripetere l'esecuzione da un account amministratore o con UAC abilitato"
    }
};

static TranlationSet _ja_jp = {
    1041,
    {
        L"Ciscoソフトウェアアップデートを完了するには、再起動が必要です。",
        L"正しく実行するには、管理者権限が必要です。管理者アカウントから再度実行するか、UACを有効にして再度実行してください"
    }
};

static TranlationSet _ko_kr = {
    1042,
    {
        L"Cisco 소프트웨어 업데이트를 완료하려면 재부팅해야 합니다.",
        L"올바르게 실행하려면 관리자 권한이 필요합니다. 관리자 계정에서 또는 UAC가 활성화된 상태에서 다시 실행하십시오."
    }
};

static TranlationSet _nl_nl = {
    1043,
    {
        L"Een update van Cisco-software vereist een herstart.",
        L"Voor dit programma heeft u beheerdersrechten nodig. Start het opnieuw met een beheerdersaccount of met UAC ingeschakeld." 
    }
};


static TranlationSet _pl_pl = {
    1045,
    {
        L"Aktualizacja oprogramowania Cisco wymaga ponownego uruchomienia komputera.",
        L"Do prawidłowego działania wymagane są uprawnienia administratora. Uruchom ponownie z konta administratora lub z włączoną funkcją UAC." 
    }
};

static TranlationSet _pt_br = {
    1046,
    {
        L"Uma atualização de software da Cisco requer uma reinicialização para ser concluída.",
        L"Os privilégios administrativos são necessários para funcionar corretamente. Execute novamente de uma conta de administrador ou com o UAC ativado" 
    }
};

static TranlationSet _ru_ru = {
    1049,
    {
        L"Для завершения обновления программного обеспечения Cisco требуется перезагрузка.",
        L"Для правильной работы требуются права администратора. Повторите запуск с учетной записью администратора или с включенным контролем учетных записей"
    }
};

static TranlationSet _zh_cn = {
    4,
    {
        L"需要重新启动才能完成思科软件更新。",
        L"需要具有管理权限才能正确运行。请从管理员账户再次运行或在启用 UAC 的情况下再次运行" 
    }
};

static TranlationSet _zh_hans = {
    2052,
    {
        L"需要重新启动才能完成思科软件更新。",
        L"需要具有管理权限才能正确运行。请从管理员账户再次运行或在启用 UAC 的情况下再次运行" 
    }
};

static TranlationSet _zh_hant = {
    31748,
    {
        L"思科軟體更新需重新開機才能完成。",
        L"需有管理權限才能正確執行。請再次從管理員帳戶或在啟用 UAC 的情況下執行" 
    }
};

static TranlationSet _zh_tw = {
    1028,
    {
        L"思科軟體更新需重新開機才能完成。",
        L"需有管理權限才能正確執行。請再次從管理員帳戶或在啟用 UAC 的情況下執行"
}
};

static std::map <int, std::vector<std::wstring>> _translationMap = {
    { _englishTranslationSet.languageCode, _englishTranslationSet.strings },
    { _cs_cz.languageCode, _cs_cz.strings },
    { _de_de.languageCode, _de_de.strings },
    { _es_es.languageCode, _es_es.strings },
    { _fr_ca.languageCode, _fr_ca.strings },
    { _fr_fr.languageCode, _fr_fr.strings },
    { _hu_hu.languageCode, _hu_hu.strings },
    { _it_it.languageCode, _it_it.strings },
    { _ja_jp.languageCode, _ja_jp.strings },
    { _ko_kr.languageCode, _ko_kr.strings },
    { _nl_nl.languageCode, _nl_nl.strings },
    { _pl_pl.languageCode, _pl_pl.strings },
    { _pt_br.languageCode, _pt_br.strings },
    { _ru_ru.languageCode, _ru_ru.strings },
    { _zh_cn.languageCode, _zh_cn.strings },
    { _zh_hans.languageCode, _zh_hans.strings },
    { _zh_hant.languageCode, _zh_hant.strings },
    { _zh_tw.languageCode, _zh_tw.strings }
};
