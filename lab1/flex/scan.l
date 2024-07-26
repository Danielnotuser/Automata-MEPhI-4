%option noyywrap
number \+[0-9]{11}
service tel|fax|(sms)
text [0-9a-zA-Z%\.\,!\?]{1,64}
%x NUM TEXT
%%
<INITIAL>{
	^({service})\:({number})+
}
