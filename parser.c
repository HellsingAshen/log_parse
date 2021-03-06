#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "logf.h"
#include "macro.h"
#include <assert.h>
#include <stdarg.h>
#include "./vector/vectorc.h"

/** for dir **/
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

/** for regex **/
#include <regex.h>

/** for rb_tree_op **/
#include "rbtree_op.h"
#include "parser.h"


/** for public func **/
#include "publicf.h"


/** 
 * @ desc : get line content
 *          example :  
 *              pcLine ->[2018-12-20 04:59:15    74042 ____clisfer.sqc:1165 accept ok]
 *              *pcLineCtx -> [accept ok]
 * @ in   : 
 *          pcLine       
 * @ out  :
 *          **ppcLineCtx
 * @ cautious: 
 *      *ppcLineCtx is not a new memory because we do need to save it.
 */
int GetLineCtt(char* pcLine, char** ppcLineCtx)
{
    int             iRet            = 0; 
    char*           pcOut           = NULL;

    iRet    = GetNthPosBySep(pcLine, 5, ' ', &pcOut);    
    RETURN_IF_ERR(iRet || !pcOut);
    
    *ppcLineCtx = pcOut;
    
    return iRet;
}


int GetContentType(char* pcLineCtx, CtxType_E* peCtxType)
{
    int             iRet            = 0;
    
    if (strlen(pcLineCtx) < 4) 
    {
        LOGE("parse line content length little than 4, skip it .");
        *peCtxType  = ERR_CODE_INVAILDLINE;
    }
    
    if (!STRCMPB(pcLineCtx, "通讯参数"))  
    {
        *peCtxType  = CT_TXCS;    
    } 
    else if (!STRCMPB(pcLineCtx, "交易准入"))
    {
        *peCtxType  = CT_JYZR;
    }
    else if (!STRCMPB(pcLineCtx, "Info"))
    {
        *peCtxType  = CT_INFO;
    }
    else if (!STRCMPB(pcLineCtx, "shma"))
    {
        *peCtxType  = CT_SHMA;
    }
    else if (!STRCMPB(pcLineCtx, "接受传输请求(获取"))
    {
        *peCtxType  = CT_TMPQ;
    }
    else if (!STRCMPB(pcLineCtx, "接受传输请求(批量"))
    {
        *peCtxType  = CT_BATQ;
    }
    else if (!STRCMPB(pcLineCtx, "接受传输请求(下载"))
    {
        *peCtxType  = CT_DWRQ;
    }
    else if (!STRCMPB(pcLineCtx, "接受传输请求(上传)"))
    {
        *peCtxType  = CT_UPRQ;
    }
    else if (!STRCMPB(pcLineCtx, "获取配置"))
    {
        *peCtxType  = CT_HQPZ;
    }
    else if (!STRCMPB(pcLineCtx, "向源系统"))
    {
        *peCtxType  = CT_XYXT;
    }
    else if (*pcLineCtx == '[')
    {
        *peCtxType  = CT_TDET;
    }
    else if (!STRCMPB(pcLineCtx, "远程连接"))
    {
        *peCtxType  = CT_YCLJ;
    }
    else if (!STRCMPB(pcLineCtx, "连接应用"))
    {
        *peCtxType  = CT_LJYY;
    }
    else if (!STRCMPB(pcLineCtx, "TcpS"))
    {
        *peCtxType  = CT_LJYY;
    }
    else if (!STRCMPB(pcLineCtx, "调用Tc"))
    {
        *peCtxType  = CT_DYTC;
    }
    else if (!STRCMPB(pcLineCtx, "文件传输"))
    {
        *peCtxType  = CT_WJCS;
    }
    else if (!STRCMPB(pcLineCtx, "上传成功"))
    {
        *peCtxType  = CT_RETS;
    }
    else if (!STRCMPB(pcLineCtx, "接收传输"))
    {
        *peCtxType  = CT_JSCS;
    }
    else if (!STRCMPB(pcLineCtx, "TcpS"))
    {
        *peCtxType  = CT_TCPS;
    }
    else if (!STRCMPB(pcLineCtx, "远程应用"))
    {
        *peCtxType  = CT_YCYY;
    }
    else if (!STRCMPB(pcLineCtx, "req fr")) 
    {
        *peCtxType  = CT_RPAG; /* response from agent */
    }
    else if (!STRCMPB(pcLineCtx, "多文件文"))
    {
        *peCtxType  = CT_RETB;
    }
    else if (!STRCMPB(pcLineCtx, "规则文件"))
    {
        *peCtxType  = CT_GZWJ;
    }
    else if (!STRCMPB(pcLineCtx, "传输返回"))
    {
        *peCtxType  = CT_CSFH;
    }
    else
    {
        LOGE("parse line content do not match anything . skip it ."
              "lint content is ---->%s<----\n",
               pcLineCtx);
        return ERR_CODE_UNRECOGNISELINE;
    }

    return iRet;
}

void SetLnCttByType(
    Vector_S*       pstVctor,
    ...
    )
{
                        
    char**          ppcEle          = NULL;

    va_list ap;
    va_start(ap, pstVctor);

    while (pstVctor->uiCount)
    {
        ppcEle = va_arg(ap, char **);
        *ppcEle = Vct_Popback(pstVctor);
    }

    va_end(ap);
    return;

}
/*
 * add set method 
 */

int SetLnCttByType_UPRQ(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    char*           pcNoUse         = NULL;

    pcNoUse    = Vct_Popback(pstVctor);
    FREE(pcNoUse); /* ns */

    
    SetLnCttByType(
        pstVctor,
        &pstLnCtt->pcTranCode,
        &pstLnCtt->pcNodeDst,
        &pstLnCtt->pcAppDst,
        &pstLnCtt->pcSrcIp
    ); 

    return 0;
}
int SetLnCttByType_INFO(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    if (1 == pstVctor->uiCount)
    {
        SetLnCttByType(
            pstVctor,
            &pstLnCtt->pcDst); 
    }
    else
    {
        SetLnCttByType(
            pstVctor,
            &pstLnCtt->pcDst,
            &pstLnCtt->pcSrc); 
    }
    return 0;
}

int SetLnCttByType_JYZR(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    if (0 == pstVctor->uiCount)
    {
        pstLnCtt->pcRetCode       = malloc(5);
        memset(pstLnCtt->pcRetCode, 0, 5);
        memcpy(pstLnCtt->pcRetCode, "9700", 4);

#if 1
        pstLnCtt->pcRetDesc       = malloc(17);
        memset(pstLnCtt->pcRetDesc, 0, 17);
        memcpy(pstLnCtt->pcRetDesc, "交易准入判断失败", strlen("交易准入判断失败"));
#endif
    }
    else if (2 == pstVctor->uiCount)
    {

        SetLnCttByType(
            pstVctor,
            &pstLnCtt->pcNodeDst, 
            &pstLnCtt->pcAppDst);
    }
    return 0;
}

int SetLnCttByType_TXCS(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    char*           pcNoUse         = NULL;

    if (!pstVctor->uiCount)
    {
    /* pro line like this 
     * 2018-12-27 11:23:27    74067 ____clisfer.sqc:1256 通讯参数获取 
     */  
        return ERR_CODE_INVAILDLINE; 
    }
    pcNoUse    = Vct_Popback(pstVctor);
    FREE(pcNoUse); /* ns */
    SetLnCttByType(
        pstVctor,
        &pstLnCtt->pcTranCode, 
        &pstLnCtt->pcNodeSrc, 
        &pstLnCtt->pcAppSrc, 
        &pstLnCtt->pcSrcIp
    ); 

    return 0;
    
}

int SetLnCttByType_TDET(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    int             iRet            = 0;

    pstLnCtt->pcDstFile     = Vct_Popback(pstVctor);
    pstLnCtt->pcSeqNo       = Vct_Popback(pstVctor);
    pstLnCtt->pcDstPort     = Vct_Popback(pstVctor);
    pstLnCtt->pcDstIp       = Vct_Popback(pstVctor);
    pstLnCtt->pcBasePath    = Vct_Popback(pstVctor);
    pstLnCtt->pcNodeDst     = Vct_Popback(pstVctor);
    pstLnCtt->pcAppDst      = Vct_Popback(pstVctor);
    return iRet;
}

int SetLnCttByType_CSFH(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    SetLnCttByType(
        pstVctor,
        &pstLnCtt->pcRetDesc, 
        &pstLnCtt->pcRetCode
    ); 

    return 0;
    
}

int SetLnCttByType_WJCS(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    SetLnCttByType(
        pstVctor,
        &pstLnCtt->pcRetDesc,
        &pstLnCtt->pcRetCode
    ); 

    return 0;
}

int SetLnCttByType_RPAG(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    SetLnCttByType(
        pstVctor,
        &pstLnCtt->pcRetCode
    ); 

    return 0;
}

int SetLnCttByType_TMPQ(
    Vector_S*       pstVctor,
    LineContent_S*  pstLnCtt
    )
{
    char*           pcNs                = NULL;
    pcNs    = Vct_Popback(pstVctor);
    FREE(pcNs); /* ns */
    SetLnCttByType(
        pstVctor,
        &pstLnCtt->pcTranCode,
        &pstLnCtt->pcSrcIp
    ); 

    return 0;
}

typedef int (*PFUNC_KEY_CB)(Vector_S* pstVctor, LineContent_S* pstLnCtt);

typedef struct tagKeyFunc_S
{
    int          iKey;
    PFUNC_KEY_CB pfnKeyCb;
}KeyFunc_S;

KeyFunc_S g_astSetX[] = {
    {CT_TDET,  SetLnCttByType_TDET},
    {CT_TXCS,  SetLnCttByType_TXCS},
    {CT_JYZR,  SetLnCttByType_JYZR},
    {CT_INFO,  SetLnCttByType_INFO},
    {CT_UPRQ,  SetLnCttByType_UPRQ},

    {CT_CSFH,  SetLnCttByType_CSFH},

    {CT_RPAG,  SetLnCttByType_RPAG},
    
    {CT_WJCS,  SetLnCttByType_WJCS},

    {CT_TMPQ,  SetLnCttByType_TMPQ},
};

/** 
 * @ desc: get [] value from pcStr and pushback into pstVct
 * @ in  :
 * @ out :
 * @ cautious:
 *
 **/

int GetSpecVec(char* pcStr, Vector_S* pstVct)
{
    int             iRet            = 0;
    char*           pcStart         = NULL;
    char*           pcEnd           = NULL;
    char*           pcPos           = NULL;
    char*           pcValue         = NULL;

    assert(NULL != pcStr || NULL != pstVct);
    
    pcPos   = pcStr;
    
    while (pcPos && ('\0' != *pcPos))
    {
        if ('[' == *pcPos && '\0' != *(pcPos + 1))
        {
            pcStart = pcPos;
        }
        if (']' == *pcPos)
        {
            pcEnd = pcPos;
        }
        if (pcStart && pcEnd && (pcEnd > pcStart))
        {
            pcValue = malloc(pcEnd - pcStart);
            memset(pcValue, 0, pcEnd - pcStart);
            memcpy(pcValue, pcStart + 1, pcEnd - pcStart - 1);
            Vct_Pushback(pstVct, pcValue);
        
            pcStart = NULL;
            pcEnd   = NULL;
        }
        pcPos++;
    }
    
    return iRet;
}

/**
 * @ desc   : update pstContent type by vectory count 
 * @ in     : pstVct
 * @ out    : pstContent
 * @ cautious:       
 **/
int UpdateTypeByVct(
    LineContent_S*  pstContent, 
    Vector_S*       pstVct
    )
{
    int             iRet            = 0;

    if (CT_JSCS == pstContent->enContentType)
    {
        if (5 == pstVct->uiCount)
        {
            pstContent->enContentType = CT_UPRQ;
        }
        else if (3 == pstVct->uiCount)
        {
            pstContent->enContentType = CT_TMPQ;
        }
        
    }

    return iRet;

}

/**
 * @ desc   :  get [] value arrary.
 *           example 
 *           in     -- [001][01][P4DTATMPAP/][10.14.138.2][9000][000361][ETFE03(S021773443)]
 *           out    -- 001  01   P4DTATMPAP/  10.10.138.2  9000 000361 ETFE03(S021773443)
 *              
 * @ in     :
 * @ out    :
 * @ cautious :
 *
 **/
int GetContentDetail(
    char*           pcContent, 
    LineContent_S*  pstLnCtt,
    Vector_S*       pstVct
    )
{
    int             iRet            = 0;
    int             i               = 0;
    
    assert(NULL != pcContent || NULL != pstVct);

    iRet = GetSpecVec(pcContent, pstVct);
    RETURN_IF_ERR(iRet);

    iRet = UpdateTypeByVct(pstLnCtt, pstVct);
    RETURN_IF_ERR(iRet);
    

    for (; i < sizeof(g_astSetX)/sizeof(KeyFunc_S); i++)
    {
        if (pstLnCtt->enContentType == g_astSetX[i].iKey)
        {
            iRet = g_astSetX[i].pfnKeyCb(pstVct, pstLnCtt); 
            return iRet;
        }
    }

    if (i == sizeof(g_astSetX)/sizeof(KeyFunc_S))
    {
        iRet    = ERR_CODE_NO_CB;
    }
       
    return iRet;
}
#define DESTRUCTCTTELE(p,ep) \
    do { \
        if (p->ep) {FREE(p->ep);} \
    } while (0);

void DestructCtt(LineContent_S* pstLnCtt)
{
    if (!pstLnCtt) 
    {
        return;
    }

    DESTRUCTCTTELE(pstLnCtt, pcTranCode);
    
    DESTRUCTCTTELE(pstLnCtt, pcRetCode);
    DESTRUCTCTTELE(pstLnCtt, pcRetDesc);
    DESTRUCTCTTELE(pstLnCtt, pcAppSrc);
    DESTRUCTCTTELE(pstLnCtt, pcAppDst);
    DESTRUCTCTTELE(pstLnCtt, pcNodeSrc);
    DESTRUCTCTTELE(pstLnCtt, pcNodeDst);
    DESTRUCTCTTELE(pstLnCtt, pcDstFile);
    DESTRUCTCTTELE(pstLnCtt, pcSeqNo);
    DESTRUCTCTTELE(pstLnCtt, pcSrcIp); 
    DESTRUCTCTTELE(pstLnCtt, pcDstIp);
    DESTRUCTCTTELE(pstLnCtt, pcDstPort);
    
    FREE(pstLnCtt);

    return;
}
#undef DESTRUCTCTTELE

void DestructCttValue(void* pData)
{
    FREE(pData);
    return;
}
/** 
 * @ desc: init pstLineCtt
 * @ in  :
 * @ out :
 * @ cautious:
 *
 **/

int InitLineContent(char* pcLineCtt, LineContent_S* pstLineCtt)
{
    int             iRet            = 0;
    CtxType_E       enCtxType        = CT_INVALID;
    Vector_S*       pstVctor        = NULL; /* save [] value */
   
    TrimLR(pcLineCtt);
    
    if (!strlen(pcLineCtt))
    {
        return ERR_CODE_INVAILDLINE;     
    }

    (void)Vct_Construct(&pstVctor, NULL);
    
    while (*pcLineCtt++ == '-');
    pcLineCtt--;

    iRet    = GetContentType(pcLineCtt, &enCtxType);
    GOTO_IF_COND(iRet, error);    
    

    pstLineCtt->enContentType = enCtxType;
    
    switch (enCtxType)
    {
        case CT_IGNO:
        case CT_SHMA:
        {
            iRet    = ERR_CODE_NONEEDPARSE;
            break;
        }
        default:
        {
            iRet  = GetContentDetail(pcLineCtt, pstLineCtt, pstVctor);
            GOTO_IF_COND(ERR_CODE_INVAILDLINE == iRet, error);
            break;
        }
        
    }
    
    Vct_Destruct(&pstVctor);
    return iRet;
     
error:
    Vct_Destruct(&pstVctor);
    LOGE("Ignore line [%s]. no need to parese or failed to find type .\n", pcLineCtt);
    return iRet;
}

void SetTransEndDate(LineEntry_S* pstLineEntry)
{
    if (!pstLineEntry)
    {
        return;
    }

    strcpy(pstLineEntry->pstLnCtt->acEndTime, pstLineEntry->acTime);
    strcpy(pstLineEntry->pstLnCtt->acEndDate, pstLineEntry->acDate);
    
    return;
}
/** 
 * @ desc : construct  line entry with line buf pcLine
 * @ in   : 
 *          pcLine          -- buf which is to be parsed.
 *          2018-12-24 04:59:03    33988 ____clisfer.sqc:1165 accept ok
 * @ out  :
 *   
 */
int InitLineEntry(char* pcLine, LineEntry_S* pstLineEntry)
{
    int             iRet            = 0;
    char*           pcOut           = malloc(MAX_BUFF_LEN_128);
    
    memset(pcOut, 0, MAX_BUFF_LEN_128);
    iRet    = GetNthEleBySep(pcLine, 1, SP, pcOut);
    GOTO_IF_COND(iRet, error);
    strcpy(pstLineEntry->acDate, pcOut);
    
    memset(pcOut, 0, MAX_BUFF_LEN_128);
    iRet    = GetNthEleBySep(pcLine, 2, SP, pcOut);
    GOTO_IF_COND(iRet, error);
    strcpy(pstLineEntry->acTime, pcOut);

    memset(pcOut, 0, MAX_BUFF_LEN_128);
    iRet    = GetNthEleBySep(pcLine, 3, SP, pcOut);
    GOTO_IF_COND(iRet, error);
    strcpy(pstLineEntry->acPid, pcOut);

    memset(pstLineEntry->acKey, 0, sizeof(pstLineEntry->acKey));
    sprintf(pstLineEntry->acKey, "%s%s%s", pstLineEntry->acDate, pstLineEntry->acTime, pstLineEntry->acPid);

    /* set end date time */
    SetTransEndDate(pstLineEntry);
            
    FREE(pcOut);
    return iRet;
error:
    FREE(pcOut);
    return iRet;
}

#if DESC(clean)
/** 
 * @ desc : destory line entry
 * @ in   :
 * @ out  :
 * @ cautious:
 *
 **/
void DestroyLineEntry(LineEntry_S* pstLineEntry)
{
    LineContent_S*      pstLnTmpCtt         = NULL;

    if (!pstLineEntry) return;

    if (!list_empty(&(pstLineEntry->stListHead)))
    {
        list_for_each_entry(pstLnTmpCtt, &(pstLineEntry->stListHead), stLink)
        {
            DestructCtt(pstLnTmpCtt);
        }
    }

    if (!list_empty(&(pstLineEntry->stListHead)))
    {
        LOGE("destory line entry error. please check it . key = [%s].\n", pstLineEntry->acKey);
    }

    pstLineEntry->pstLnCtt  = NULL;

    FREE(pstLineEntry);
    return;
}

#endif

/** 
 * @ desc : construct  line entry with line buf
 * @ in   : 
 *          pcLine          -- buf which is to be parsed.
 * @ out  :
 *          **ppstLineEntry -- line entry which is construct by line buf.
 * @ cautious:
 *
 **/
int ConstructLineEntry(char* pcLine, LineEntry_S** ppstLineEntry)
{
    int             iRet            = 0;
    LineEntry_S*    pstLineEntry    = NULL;

    char*           pcLineCtt       = NULL;
    LineContent_S*  pstLineContent  = NULL;

    /* pcLinectt point to pcLine + x, don't need to free it */
    iRet    = GetLineCtt(pcLine, &pcLineCtt);
    GOTO_IF_COND(iRet, error1);

    pstLineContent  = MALLOC(sizeof(LineContent_S));
    SETRET_GOTO_IF_COND(!pstLineContent, ERR_CODE_MALLOCFAILED, error1);

    memset(pstLineContent, 0, sizeof(*pstLineContent));

    iRet    = InitLineContent(pcLineCtt, pstLineContent);
    GOTO_IF_COND(iRet, error1);

    pstLineEntry    = MALLOC(sizeof(LineEntry_S));
    SETRET_GOTO_IF_COND(!pstLineEntry, ERR_CODE_MALLOCFAILED, error1);

    memset(pstLineEntry, 0, sizeof(*pstLineEntry));

#if 0
    /* Compile error. I don't know  the reason. */
    pstLineEntry->stListHead = LIST_HEAD_INIT(pstLineEntry->stListHead);
    pstLineEntry->stListHead = {&(pstLineEntry->stListHead), &(pstLineEntry->stListHead)};
#endif
    
    pstLineEntry->stListHead.next = &(pstLineEntry->stListHead);
    pstLineEntry->stListHead.prev = &(pstLineEntry->stListHead);

    list_add_head(&(pstLineContent->stLink), &(pstLineEntry->stListHead));

    pstLineEntry->pstLnCtt  = pstLineContent;
    iRet    = InitLineEntry(pcLine, pstLineEntry);
    GOTO_IF_COND(iRet, error);

    *ppstLineEntry  = pstLineEntry;

    return iRet;

error:
    DestroyLineEntry(pstLineEntry);
    *ppstLineEntry  = NULL;
    return iRet;

error1:
    DestructCtt(pstLineContent);
    *ppstLineEntry  = NULL;
    return iRet;
}

void PrintEntry(LineEntry_S* pstEntry)
{
    int                 iCount              = 0;
    LineContent_S*      pstLnTmpCtt         = NULL;

    if (!pstEntry)
    {
        return;
    }
    printf("--------->> Entry start---\n");
    printf("acDate      = [%s].\n", pstEntry->acDate      );
    printf("acTime      = [%s].\n", pstEntry->acTime      );
    printf("acPid       = [%s].\n", pstEntry->acPid       );
    printf("acKey       = [%s].\n", pstEntry->acKey       );

    if (!list_empty(&(pstEntry->stListHead)))
    {
        list_for_each_entry(pstLnTmpCtt, &(pstEntry->stListHead), stLink)
        {
            printf("Entry list ###[%d]###\n", iCount++);
            printf("   enContentType= [%d]\n", pstLnTmpCtt->enContentType);
            printf("   acEndDate    = [%s]\n", pstLnTmpCtt->acEndDate  );
            printf("   acEndTime    = [%s]\n", pstLnTmpCtt->acEndTime  );

            printf("   pcTranCode   = [%s]\n", pstLnTmpCtt->pcTranCode );
            printf("   pcRetCode    = [%s]\n", pstLnTmpCtt->pcRetCode  );
            printf("   pcRetDesc    = [%s]\n", pstLnTmpCtt->pcRetDesc  );
            printf("   pcAppSrc     = [%s]\n", pstLnTmpCtt->pcAppSrc   );
            printf("   pcAppDst     = [%s]\n", pstLnTmpCtt->pcAppDst   );
            printf("   pcNodeSrc    = [%s]\n", pstLnTmpCtt->pcNodeSrc  );
            printf("   pcNodeDst    = [%s]\n", pstLnTmpCtt->pcNodeDst  );
            printf("   pcDstFile    = [%s]\n", pstLnTmpCtt->pcDstFile  );
            printf("   pcSeqNo      = [%s]\n", pstLnTmpCtt->pcSeqNo    );
            printf("   pcSrcIp      = [%s]\n", pstLnTmpCtt->pcSrcIp    );
            printf("   pcDstIp      = [%s]\n", pstLnTmpCtt->pcDstIp    );
            printf("   pcDstPort    = [%s]\n", pstLnTmpCtt->pcDstPort  );
            printf("   pcBasePath   = [%s]\n", pstLnTmpCtt->pcBasePath );
            printf("   pcDst        = [%s]\n", pstLnTmpCtt->pcDst      );
            printf("   pcSrc        = [%s]\n", pstLnTmpCtt->pcSrc      );
        }
        
    }

    printf("---------<<< Entry end---\n");
    return;
}

/** 
 * @ desc : process line which is first line of a transaction.
 * @ in   :
 * @ out  :
 * @ cautious: insert if not exist . add into list if exist.
 *
 **/
int TransBeginPro(
    struct rb_root* pstRoot,  
    LineEntry_S*    pstLnEty
    )
{
    int             iRet            = 0;
    LineEntry_S*    pstLineEntry    = NULL;
    struct  list_head* pstLink      = NULL;
   
    pstLineEntry = SerachNode(pstRoot, pstLnEty->acKey);
    if (!pstLineEntry)
    {
        iRet    = InsertNode(pstRoot, pstLnEty);
    }
    else
    {
        pstLink = &(pstLnEty->pstLnCtt->stLink);

        /* transfer list node from new to old */
        list_del(&(pstLnEty->pstLnCtt->stLink));
        list_add_head(pstLink, &(pstLineEntry->stListHead));

        /*  reset latest line content ptr */
        pstLineEntry->pstLnCtt = pstLnEty->pstLnCtt;

        pstLnEty->pstLnCtt = NULL;
        DestroyLineEntry(pstLnEty);
    }
    return iRet;
}

/** 
 * @ desc : update tree by new line entry 
 * @ in   :
 * @ out  :
 * @ cautious:
 *
 **/
int TransUpdate(
    struct rb_root* pstRoot,  
    LineEntry_S*    pstEntryNew
    )
{
    int             iRet            = 0;
    LineEntry_S*    pstLineEntry    = NULL;

    assert(NULL != pstRoot || NULL != pstEntryNew);
    
    /* do serach */
    pstLineEntry = SerachNode(pstRoot, pstEntryNew->acKey);

    if (!pstLineEntry)
    {
        /**  fuzzy serach again and not found , error **/
        iRet    = FuzzySerachNode(pstRoot, pstEntryNew, &pstLineEntry);
        if(iRet || NULL == pstLineEntry)
        {
            LOGE("fuzzy serach find failed , please check it . key = [%s]\n", pstLineEntry->acKey);    
            return iRet;
        }
    }
    
    iRet    = RefreshNode(pstEntryNew, pstLineEntry);
    RETURN_IF_ERR(iRet);
    
    return  iRet;
}

/** 
 * @ desc : parser file 
 * @ in   :
 * @ out  :
 * @ cautious:
 *
 **/
int ParseFile(char* pcFile)
{
    int             iRet            = 0;
    FILE*           fpLog           = NULL;
    char*           pcLine          = NULL;
    size_t          tLen            = 0;
    ssize_t         tReadLine       = 0;
    struct rb_root  stCSTree        = RB_ROOT;
    LineEntry_S*    pstLnEty        = NULL;

    struct rb_node* pstNode         = NULL;

    assert (pcFile != NULL);
    
    LOGD("start parse file [%s].\n", pcFile);
    
    assert(NULL != (fpLog = fopen(pcFile, "r")));
   
    /* construct tree */
    while (-1 != (tReadLine = getline(&pcLine, &tLen, fpLog))) 
    {
        iRet    = ConstructLineEntry(pcLine, &pstLnEty);
        printf("parse line [%s] iRet = [%d].\n", pcLine, iRet);
        if (iRet)
        {
            LOGE("Parser line [%s] failed . please check it .\n", pcLine);
            DestroyLineEntry(pstLnEty);
            FREE(pcLine);
            continue;
        }
        
        /* insert into stCSTREE */ 
        switch(pstLnEty->pstLnCtt->enContentType)
        {
#define TRANS_BEGIN
            case CT_TXCS:
            case CT_UPRQ:
            {
                iRet    = TransBeginPro(&stCSTree, pstLnEty);
                if (iRet) /* if failed , destory line entry .*/
                {
                    LOGE("Insertnode error: key = [%s].\n", pstLnEty->acKey);
                    DestroyLineEntry(pstLnEty);
                }
                break;
            }
#undef  TRANS_BEGIN
            
            default:
            {
                iRet    = TransUpdate(&stCSTree, pstLnEty);
                if (iRet)
                {
                    LOGE("TransUpdate node error: key = [%s].\n", pstLnEty->acKey);
                }
                DestroyLineEntry(pstLnEty);
                break;
            }
        } 

        FREE(pcLine);
    }

    /* insert into db by tree node */
    /* TODO: add your operation by stScTree */

    for (pstNode = rb_first(&stCSTree); pstNode;  pstNode = rb_next(pstNode))
    {
        pstLnEty    = container_of(pstNode, LineEntry_S, stNode);
        PrintEntry(pstLnEty);
    }

    
    /* clean env */
    for (pstNode = rb_first(&stCSTree); pstNode;  pstNode = rb_next(pstNode))
    {
        pstLnEty    = container_of(pstNode, LineEntry_S, stNode);
        rb_erase(&pstLnEty->stNode, &stCSTree);
        DestroyLineEntry(pstLnEty);
    }
    
    FREE(pcLine);
    fclose(fpLog);
    return iRet;
}

/** 
 * @ desc: checking  file_name  is fitting for format.
 * @ in  :
 * @ out :
 * @ cautious:
 *
 **/
int CheckParserFileName(char* pcFileName, char* pcFormat)
{
    int             iRet            = 0;
    regex_t         tRegex             ;
    
    assert((pcFileName != NULL) || (pcFormat != NULL));
    
    do 
    {
        iRet    = regcomp(&tRegex, pcFormat, REG_EXTENDED);
        BREAK_IF_ERR(iRet);
        
        iRet    = regexec(&tRegex, pcFileName, 0, NULL, 0);
        BREAK_IF_ERR(iRet);
    } while (0);

    regfree(&tRegex);
    return iRet;
}


/**
 * @ desc   : check file path 
 * @ in     : pcPath
 * @ out    :
 * @ cautious:
 **/
int CheckFilePath(char* pcPath)
{
    int         iRet            = 0;
    struct stat stInfo;

    assert(NULL != pcPath);
    iRet    = ReplaceEnvVar(pcPath);
    RETURN_IF_ERR(iRet);

    (void)stat(pcPath, &stInfo);
    iRet    = S_ISDIR(stInfo.st_mode);
    RETURN_IF_ERR(!iRet);
    
    iRet    = access(pcPath, R_OK);
    RETURN_IF_ERR(iRet);

    return iRet;
}

/**
 * @ desc   : check file path 
 * @ in     : pcPath
 * @ out    :
 * @ cautious:
 **/
int ParseDir(char* pcPath)
{
    int         iRet            = 0;

    DIR*        pDir            = NULL;
    struct dirent* pstDirEle    = NULL;
    
    char        acFullFilePath[MAX_PATH_LEN + 1] = {0};
    char        acPatton[MAX_PATH_LEN]           = "\\w{3,30}\\.[2-9][0-9]{7}"; /** only process filename like abc.20111111 **/

    /** check file path aviable **/
    iRet    = CheckFilePath(pcPath);
    RETURN_IF_ERR(iRet);
    
    assert(NULL != (pDir = opendir(pcPath)));

    while ((pstDirEle   = readdir(pDir)))
    {
        /** ignore . , .. and .*.swx  **/
        if ('.' == pstDirEle->d_name[0])
        {
            continue;
        }

        /* cur pro file type is [clisfre.date] */
        /** parse file **/
        {
            memset(acFullFilePath, 0, sizeof(acFullFilePath));
            sprintf(acFullFilePath, "%s/%s", pcPath, pstDirEle->d_name);

            iRet    = CheckParserFileName(pstDirEle->d_name, acPatton);
            if (iRet) 
            {
                printf("ignore file [%s]\n", pstDirEle->d_name);
                LOGD("do not parser file [%s] because of invalid file_name format.\n", pstDirEle->d_name);
                continue;
            }
            
            iRet    = ParseFile(acFullFilePath);
            BREAK_IF_ERR(iRet);
        }
    }

    closedir(pDir);
    return  iRet;
}
