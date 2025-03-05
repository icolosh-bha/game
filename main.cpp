#include<bits/stdc++.h>
using namespace std;
bool cmp(pair<int,int>a,pair<int,int> b)
{
    return a.second<b.second;
}
int main()
{
    freopen("test.inp","r",stdin);
    int n,m;
    cin>>n>>m;
    vector<pair<int,int>>a(m);
    for(int i=0;i<m;i++)
        cin>>a[i].first>>a[i].second;
    sort(a.begin(),a.end(),cmp);
  int sum=0,res=0,i=0;
  for(i=0;i<m;i++)
    if(sum+a[i].first<=n) {sum+=a[i].first;res+=(a[i].first)*(a[i].second);} else break;
if(sum<=n) {res+=(n-sum)*a[i++].second;}
  cout<<res<<endl;
  for(int j=0;j<m;j++)
  {
      if(j<i) cout<<a[j].first<<endl;
      else
        if(j==i) cout<<n-sum<<endl;
      else
        cout<<0<<endl;
  }
    /*int sum=0,res=0,i=0;
    while(sum<n)
    {
        if(sum+a[i].first<=n) {sum+=a[i].first;res+=(a[i].first)*(a[i].second);i++;}
    }
    cout<<res<<" "<<sum;*/
}
